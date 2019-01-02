#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>
#include <vector>

#define MAX_SIZE 10000

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using list_iterator = typename std::list<value_type>::iterator;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
    //std::vector <value_type> *tab;
    std::list <value_type> *tab;
    //size_type first, last;
    size_type size;
    //std::hash <key_type> hashKey;


public:

  /*int getHash(const key_type &key) {
    return std::hash<key_type> ()(key)%MAX_SIZE;
  }*/

  size_type getHash(const key_type &key) const{
   // #ifdef OperationCountingObject
        return key% MAX_SIZE;
    /*#else
        return std::hash<key_type>()(key) % MAX_SIZE;
    #endif*/
}

 /*void append(const const_iterator &it) {
    tab[getHash(it->first)].push_back(it->second);
    size++;
  }*/

  void append(const value_type data) {
    tab[getHash(data.first)].push_back(data);
    size++;
  }

  HashMap() {
    size = 0;
    tab = new std::list <value_type> [MAX_SIZE];
  }

  ~HashMap() {
  }

  HashMap(std::initializer_list<value_type> list) : HashMap() {
    for(auto it = list.begin(); it != list.end(); it++) {
        append(std::make_pair(it->first, it->second));
    }
  }

  HashMap(const HashMap& other) : HashMap() {
    for(auto it = other.begin(); it != other.end(); it++) {
        append(std::make_pair(it->first, it->second));
    }
  }

  HashMap(HashMap&& other) {
    this->size = other.size;
    this->tab = other.tab;
    other.tab = nullptr;
    other.size = 0;
  }

  HashMap& operator=(const HashMap& other) {
    if(this == &other)
        return *this;
    size_type i = 0;
    while(i < MAX_SIZE) {
        tab[i].clear();
        i++;
    }
    //delete[] tab;
    size = 0;
    for(auto it = other.begin(); it != other.end(); i++)
        append(std::make_pair((*it).first, (*it).second));
    return *this;
  }

  HashMap& operator=(HashMap&& other) {
    if(this == &other)
        return *this;
    size_type i = 0;
    while(i < MAX_SIZE) {
        tab[i].clear();
        i++;
    }
    delete[] tab;
    this->size = other.size;
    this->tab = other.tab;
    other.tab = nullptr;
    other.size = 0;
    return *this;
  }

  bool isEmpty() const {
    return (size == 0);
  }

  mapped_type& operator[](const key_type& key) {
    size_type hashedKey = getHash(key);
    list_iterator list_it = tab[hashedKey].begin();
    while (list_it != tab[hashedKey].end()) {
        if((*list_it).first == key)
            return (*list_it).second;
    }
    append(std::make_pair(key, mapped_type()));
    return tab[getHash(key)].back().second;
    //append()
  }

  const mapped_type& valueOf(const key_type& key) const {
    (void)key;
    throw std::runtime_error("TODO");
  }

  mapped_type& valueOf(const key_type& key) {
    (void)key;
    throw std::runtime_error("TODO");
  }

  const_iterator find(const key_type& key) const {
    size_type hashedKey = getHash(key);
    list_iterator list_it = tab[hashedKey].begin();
    if(tab[hashedKey].empty())
        return const_iterator(this, MAX_SIZE, tab[0].begin());
    else {
        while(list_it != tab[hashedKey].end()) {
            if((*list_it).first == key)
                break;
            list_it++;
        }
    }
    return const_iterator(this, hashedKey, list_it);
  }

  iterator find(const key_type& key) {
    size_type hashedKey = getHash(key);
    list_iterator list_it = tab[hashedKey].begin();
    if(tab[hashedKey].empty())
        return iterator(this, MAX_SIZE, tab[0].begin());
    else {
        while(list_it != tab[hashedKey].end()) {
            if((*list_it).first == key)
                break;
            list_it++;
        }
    }
    return iterator(this, hashedKey, list_it);
  }

  void remove(const key_type& key) {
    if(isEmpty())
        throw std::out_of_range("Trying to remove from empty");
    if(tab[getHash(key)].empty())
        throw std::out_of_range("Trying to reach non existent key");
    auto it = tab[getHash(key)].begin();
    auto end = tab[getHash(key)].end();
    while(it != end) {
        if(key == it->first) {
            tab[getHash(key)].erase(it);
            size--;
            return;
        }
        it++;
    }
    throw std::out_of_range("Trying to remove non existent value");
  }

  void remove(const const_iterator& it) {
    if(isEmpty())
        throw std::out_of_range("Trying to remove from empty");
    if(it == end())
        throw std::out_of_range("Trying to remove end()");
    remove((*it).first);
  }

  size_type getSize() const {
    return size;
  }

  bool operator==(const HashMap& other) const {
    (void)other;
    throw std::runtime_error("TODO");
  }

  bool operator!=(const HashMap& other) const {
    return !(*this == other);
  }

  iterator begin() {
    if(isEmpty())
        return iterator(this, MAX_SIZE, tab[0].end());
    size_type i = 0;
    while(i < MAX_SIZE) {
        if(tab[i].empty())
            i++;
        else
            break;
    }
    return iterator(this, i, tab[i].begin());
  }

  iterator end() {
    return iterator(this, MAX_SIZE, tab[0].end());
  }

  const_iterator cbegin() const {
    if(isEmpty())
        return const_iterator(this, MAX_SIZE, tab[0].end());
    size_type i = 0;
    while(i < MAX_SIZE) {
        if(tab[i].empty())
            i++;
        else
            break;
    }
    return const_iterator(this, i, tab[i].begin());
  }

  const_iterator cend() const {
    return const_iterator(this, MAX_SIZE, tab[0].end());
  }

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator end() const {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;
  using list_iterator = typename std::list<value_type>::iterator;

private:
  const HashMap *map;
  size_type tabIndex;
  list_iterator list_it;
  //std::vector <std::pair<key_type, mapped_type> > *tab;

  friend void HashMap <KeyType, ValueType>::remove(const const_iterator& it);

public:

  explicit ConstIterator(const HashMap *map, size_type tabIndex, list_iterator list_it) : map(map), tabIndex(tabIndex), list_it(list_it) {
    /*this->map = map;
    this->tabIndex = tabIndex;
    this->list_it = list_it;*/
  }

  ConstIterator(const ConstIterator& other) {
    this->map = other.map;
    this->tabIndex = other.tabIndex;
    this->list_it = other.list_it;
  }

  ConstIterator& operator++() {
    if(map->isEmpty())
        throw std::out_of_range("Cannot increment while map is empty");
    if(tabIndex == MAX_SIZE)
        throw std::out_of_range("Trying to increment end");
    list_it++;
    if(map->tab[tabIndex].end() == list_it) {
        tabIndex++;
        while(tabIndex < MAX_SIZE) {
            if(map->tab[tabIndex].empty())
                tabIndex++;
            else {
                list_it = map->tab[tabIndex].begin();
                return *this;
            }
        }
        // if(tabIndex == MAX_SIZE)
    }
    if(tabIndex == MAX_SIZE)
        list_it = map->tab[0].end();
    return *this;
  }

  ConstIterator operator++(int) {
    auto result = *this;
    operator++();
    return result;
  }

  ConstIterator& operator--() {
    if(map->isEmpty())
        throw std::out_of_range("Cannot decrement while map is empty");
    if(*this == map->begin())
        throw std::out_of_range("trying to decrement begin");
    list_it--;
    if(tabIndex == MAX_SIZE) {
        tabIndex--;
        while(tabIndex > 0) { //another duplication of code
            if(map->tab[tabIndex].empty())
                tabIndex--;
            else {
                list_it = map->tab[tabIndex].end();
                list_it--;
                break;
            }
        }
        return *this; //we earlier checked if map is empty
    }
    if(list_it != map->tab[tabIndex].begin())
        list_it--;
    else {
        tabIndex = (tabIndex - 1)%MAX_SIZE;
        while(tabIndex != MAX_SIZE) {
            if(map->tab[tabIndex].empty())
                tabIndex = (tabIndex - 1)%MAX_SIZE;
            else
                break;
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
    if(tabIndex == MAX_SIZE)
        throw std::out_of_range("Value unavaible");
    return *list_it;
  }

  pointer operator->() const {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const {
    return ((other.list_it == this->list_it) && (other.tabIndex == this->tabIndex) && (other.map == this->map));
  }

  bool operator!=(const ConstIterator& other) const {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;
  using list_iterator = typename std::list<value_type>::iterator;

  explicit Iterator()
  {}

  Iterator(const HashMap *map, size_type tabIndex, list_iterator list_it) : ConstIterator(map, tabIndex, list_it){}

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

#endif /* AISDI_MAPS_HASHMAP_H */
