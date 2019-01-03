#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{

public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;


struct Node {

public:
	Node *up;
	Node *left;
	Node *right;
	value_type data;
	int bf;

	/*Node() {
    left = nullptr;
    right = nullptr;
    up = nullptr;
	}*/

	~Node() {
    left = nullptr;
    right = nullptr;
    up = nullptr;
	}

	Node(const KeyType& key, const ValueType& value)
	: up(nullptr), left(nullptr), right(nullptr), data(std::make_pair(key, value)), bf(0)
	{}

	Node(const KeyType& key) : Node(key, mapped_type()) {}

  //Node(const KeyType& key) :data(std::make_pair(key, mapped_type())) {}

	void setBf(int bf) {
        this->bf = bf;
	}

    void changeBf(int value) {
        this->bf = this->bf + value;
	}

	int getBf() {
        return bf;
	}
};

private:
  Node *root;
  size_t size;



public:

  TreeMap() {
    root = nullptr;
    size = 0;
  }

  ~TreeMap() {
    while(begin() != end())
        remove(begin());
  }

  TreeMap(std::initializer_list<value_type> list) : TreeMap() {
    for(auto it = list.begin(); it != list.end(); ++it) {
      (*this)[it->first] = it->second;
    }
  }

  TreeMap(const TreeMap& other) : TreeMap() {
    for(auto it = other.begin(); it != other.end(); ++it) {
      (*this)[it->first] = it->second;
    }
  }

  TreeMap(TreeMap&& other) : TreeMap() {
    this->root = other.root;
    this->size = other.getSize();
    other.size = 0;
    other.root = nullptr;
  }

  TreeMap& operator=(const TreeMap& other) {
    if (&other == this)
      return *this;
    size_t oldSize = size;
    for(size_t i = 0; i < oldSize; ++i) {
        remove(this->begin());
    }
    for(auto it = other.begin(); it != other.end(); it++) {
      (*this)[it->first] = it->second;
    }
    return *this;
  }

  TreeMap& operator=(TreeMap&& other) {
    while(!isEmpty()) {
      remove (begin());
    }
    root = other.root;
    size = other.size;
    other.root = nullptr;
    other.size = 0;
    return *this;
  }

  bool isEmpty() const {
    return (size == 0);
  }

  mapped_type& operator[](const key_type& key) {
    if(isEmpty()) {
      root = new Node(key);
      size++;
      return root->data.second;
    }

    Node *temp = root;
    Node *prev;
    do {
      prev = temp;
      if(key == temp->data.first)
        return temp->data.second;
      if(key > temp->data.first)
        temp = temp->right;
      else
        temp = temp->left;
    }while(temp != nullptr);

    temp = new Node(key);
    if(key > prev->data.first)
      prev->right = temp;
    else
      prev->left = temp;
    temp->up = prev;
    raiseBF(temp);
    balance(temp);
    size++;
    return temp->data.second;
  }

  const mapped_type& valueOf(const key_type& key) const {
    Node *temp = root;
    while(temp != nullptr) {
      if(temp->data.first == key)
        return temp->data.second;
      if(temp->data.first > key)
        temp = temp->right;
      else
        temp = temp->left;
    }
    throw std::out_of_range("Tried to reach non existent key");
  }

  mapped_type& valueOf(const key_type& key) { //Not sure if I should just use it for const also or what
    Node *temp = root;
    while(temp != nullptr) {
      if(temp->data.first == key)
        return temp->data.second;
      if(temp->data.first > key)
        temp = temp->right;
      else
        temp = temp->left;
    }
    throw std::out_of_range("Tried to reach non existent key");
  }

  const_iterator find(const key_type& key) const {
    Node *temp = root;
    while(temp != nullptr) {
      if(temp->data.first == key)
        return const_iterator(temp, this);
      if(temp->data.first < key)
        temp = temp->right;
      else
        temp = temp->left;
    }
    return const_iterator(temp, this);
    //throw std::out_of_range("Tried to find non existent key");
  }

  iterator find(const key_type& key) {
    Node *temp = root;
    while(temp != nullptr) {
      if(temp->data.first == key)
        return iterator(temp, this);
      if(temp->data.first < key)
        temp = temp->right;
      else
        temp = temp->left;
    }
    return iterator(temp, this);
    //throw std::out_of_range("Tried to find non existent key");
  }

  void remove(const key_type& key) {
    remove (find(key));
  }

  void remove(const const_iterator& it) {
    Node *toDelete = it.current;
    if(toDelete == nullptr)
      throw std::out_of_range("Trying to remove nothing");

    //case when toDeleft has no sons
    if(toDelete->left == nullptr && toDelete->right == nullptr) {
      if(toDelete != root) {
        if(toDelete->up->left == toDelete)
          toDelete->up->left = nullptr;
        else
          toDelete->up->right = nullptr;
        raiseBF(toDelete);
        balance(toDelete->up);
      }
      else
        root = nullptr;
      delete toDelete;
      size--;
      return;
    }

    //case when toDelete has only one son
    if(toDelete->left != nullptr || toDelete->right != nullptr) {
      Node *kid;
      if(toDelete->left != nullptr)
        kid = toDelete->left;
      else
        kid = toDelete->right;

      if(toDelete != root) {
        if(toDelete->up->left == toDelete)
          toDelete->up->left = kid;
        else
          toDelete->up->right = kid;
        kid->up = toDelete->up;
      }
      else {
        root = kid;
        kid->up = nullptr;
      }
      raiseBF(kid);
      delete toDelete;
      size--;
      return;
    }

    //At this point we know toDelete has two sons
    Node *toSwap = toDelete->left;
    while(toSwap->right != nullptr) //searching for largest node of left branch
      toSwap = toSwap->right;
    Node *kid = toSwap->left;
    if(kid != nullptr) {
      if(toDelete->left != toSwap) {
        if(toSwap->up->left == toSwap)
          toSwap->up->left = kid;
        else
          toSwap->up->right = kid;
      kid->up = toSwap->up;
      toSwap->left = nullptr;
      }
    }
    else {
      if(toSwap->up != nullptr) {
        if(toSwap->up->left == toSwap)
          toSwap->up->left = nullptr;
        else
          toSwap->up->right = nullptr;
      }
    }

      toSwap->left = toDelete->left;
      toSwap->right = toDelete->right;
      toDelete->left->up = toSwap;
      toDelete->right->up = toSwap;

    if(toDelete != root) {
      toSwap->up = toDelete->up;
      if(toDelete->up->left == toDelete)
        toDelete->up->left = toSwap;
      else
        toDelete->up->right = toSwap;
    }
    else {
      root = toSwap;
      toSwap->up = nullptr;
    }
      raiseBF(kid);
      delete toDelete;
      size--;

    /*if(toDelete->left == nullptr && toDelete->right != nullptr) {
      return;
    }*/
  }

  size_type getSize() const {
    return size;
  }

  bool operator==(const TreeMap& other) const {
    if(&other == this)
      return true;
    if(other.getSize() != getSize())
      return false;
    if(size == 0)
      return true;
    for(iterator i1 = cbegin(), i2 = other.cbegin(); i1 != cend() || i2 != other.cend(); i1++, i2++) {
      if((i1->first != i2->first) || (i1->second != i2->second))
        return false;
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const {
    return !(*this == other);
  }

  iterator begin() {
    if(isEmpty())
      return iterator(nullptr, this);
    Node *temp = root;
    while(temp != nullptr) {
      if(temp->left == nullptr)
        break;
      else
        temp = temp->left;
    }
    return iterator(temp, this);
  }

  iterator end() {
    return iterator(nullptr, this);
  }

  const_iterator cbegin() const {
    if(isEmpty())
      return const_iterator(nullptr, this);
    Node *temp = root;
    while(temp != nullptr) {
      if(temp->left == nullptr)
        break;
      else
        temp = temp->left;
    }
    return const_iterator(temp, this);
  }

  const_iterator cend() const {
    return const_iterator(nullptr, this);
  }

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator end() const {
    return cend();
  }

//BALANCE SECTION

  void balance(Node *temp) {
    while(temp != nullptr) {
      if(temp->bf == 2) {
        if(temp->right->bf == -1)
          rotationRL(temp);
        else
          rotationRR(temp);
      }
      else if(temp->bf == -2) {
        if(temp->left->bf == 1)
          rotationLR(temp);
        else
          rotationLL(temp);
      }
      temp = temp->up;
    }
  }

  void rotationLL(Node *a) {
    Node *b = a->left;
    if(a != root) {
      if(a->up->right == a)
        a->up->right = b;
      else
        a->up->left = b;
    }
    else
      root = b;
    if(b->right != nullptr) {
      b->right->up = a;
      a->left = b->right;
    }
    b->right = a;
    a->up = b;
    //SETTING BF
    if(b->bf == -1) {
      a->bf = 0;
      b->bf = 0;
    }
    else {
      a->bf = -1;
      b->bf = 1;
    }
    lowerBF(b);
  }

  void rotationRR(Node *a) {
    Node *b = a->right;
    b->up = a->up;
    if(a != root) {
      if(a->up->right == a)
        a->up->right = b;
      else
        a->up->left = b;
    }
    else
      root = b;
    if(b->left != nullptr) {
      b->left->up = a;
      a->right = b->left;
    }
    b->left = a;
    a->up = b;
    //SETTING BF
    if(b->bf == 1) {
      a->bf = 0;
      b->bf = 0;
    }
    else {
      a->bf = 1;
      b->bf = -1;
    }
    lowerBF(b);
  }

  void rotationLR(Node *a) {
    Node *b = a->left;
    Node *c = b->right;
    c->up = a->up;
    if(a != root) {
      if(a->up->right == a)
        a->up->right = c;
      else
        a->up->left = c;
    }
    else {
      root = c;
    }
    if(c->left != nullptr) {
      c->left->up = b;
      b->right = c->left;
    }
    if(c->right != nullptr) {
      c->right->up = a;
      a->left = c->right;
    }
    c->left = b;
    c->right = a;
    //SETTING BF
    if(c->bf == -1) {
      a->bf = 1;
      b->bf = 0;
    }
    else if(c->bf == 0) {
      a->bf = 0;
      b->bf = 0;
    }
    else {
      a->bf = 0;
      b->bf = -1;
    }
    c->bf = 0;
    lowerBF(c);
  }

  void rotationRL(Node *a) {
    Node *b = a->right;
    Node *c = b->left;
    c->up = a->up;
    if(a != root) {
      if(a->up->right == a)
        a->up->right = c;
      else
        a->up->left = c;
    }
    else {
      root = c;
    }
    if(c->left != nullptr) {
      c->left->up = a;
      a->right = c->left;
    }
    if(c->right != nullptr) {
      c->right->up = b;
      b->left = c->right;
    }
    c->left = a;
    c->right = b;
    //SETTING BF
    if(c->bf == -1) {
      a->bf = 0;
      b->bf = 1;
    }
    else if(c->bf == 0) {
      a->bf = 0;
      b->bf = 0;
    }
    else {
      a->bf = -1;
      b->bf = 0;
    }
    c->bf = 0;
    lowerBF(c);
  }

  void lowerBF(Node *temp) {
    while(temp->up != nullptr) {
      if(temp->up->bf != 0) {
        temp->up->bf = 0;
        break;
      }
      if(temp->up->right == temp) {
        temp = temp->up;
        temp->changeBf(-1);
      }
      else {
        temp = temp->up;
        temp->changeBf(1);
      }
    }
  }

  void raiseBF(Node *temp) {
    while(temp->up != nullptr) {
      if(temp->up->bf != 0) {
        temp->up->bf = 0;
        break;
      }
      if(temp->up->right == temp) {
        temp = temp->up;
        temp->changeBf(1);
      }
      else {
        temp = temp->up;
        temp->changeBf(-1);
      }
    }
  }

};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
private:
  Node *current;
  const TreeMap *currentMap;

  friend void TreeMap <KeyType, ValueType>::remove(const const_iterator& it);

public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

  explicit ConstIterator() {
    current = nullptr;
    currentMap = nullptr;
  }

  explicit ConstIterator(Node *node, const TreeMap *map) {
    current = node;
    currentMap = map;
  }

  ConstIterator(const ConstIterator& other) {
    current = other.current;
  }

  ConstIterator& operator++() { //preincremenattion
    if(*this == currentMap->cend())
      throw std::out_of_range("Cannot increment end");
    if(current->right != nullptr) {
      current = current->right;
        while(current->left != nullptr)
          current = current->left;
    }
    else {
      if(current->up == nullptr) {
        current = nullptr;
        return *this;
      }
      if(current->up->left == current) {
        current = current->up;
      }
      else {
        while((current->up != nullptr) && (current->up->right == current))
          current = current->up;
        if(current->up == nullptr)
          current = nullptr;
        else
          current = current->up;
      }
    }
    return *this;
  }

  ConstIterator operator++(int) { //postincrementation
    auto result = *this;
    operator++();
    return result;
  }

  ConstIterator& operator--() {
    if(*this == currentMap->cbegin())
      throw std::out_of_range("Cannot decrement begin");
    if(*this == currentMap->end()) {
        current = currentMap->root;
        while(current->right != nullptr)
          current = current->right;
        return *this;
    }
    if(current->left != nullptr) {
      current = current->left;
        while(current->right != nullptr)
          current = current->right;
    }
    else {
      if(current->up == nullptr) {
        current = nullptr;
        return *this;
      }
      if(current->up->right == current) {
        current = current->up;
      }
      else {
        while((current->up != nullptr) && (current->up->left == current))
          current = current->up;
        if(current->up == nullptr)
          current = nullptr;
        else
          current = current->up;
      }
    }
    return *this;
  }

  ConstIterator operator--(int) {
    auto result = *this;
    operator--();
    return result;
  }

  reference operator*() const {
    if(current == nullptr)
      throw std::out_of_range("Trying to reference to nullptr");
    return current->data;
  }

  pointer operator->() const {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const {
    return (this->current == other.current);
  }

  bool operator!=(const ConstIterator& other) const {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator()
  {}

  Iterator(Node *node, TreeMap *map) : ConstIterator(node, map){}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++() {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int) {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--() {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int) {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const {
    return &this->operator*();
  }

  reference operator*() const {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_MAP_H */
