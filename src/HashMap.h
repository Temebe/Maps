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

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
    std::vector <value_type> tab[MAX_SIZE];
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
    (void)other;
    //throw std::runtime_error("TODO");
  }

  HashMap& operator=(const HashMap& other) {
    (void)other;
    throw std::runtime_error("TODO");
  }

  HashMap& operator=(HashMap&& other) {
    (void)other;
    throw std::runtime_error("TODO");
  }

  bool isEmpty() const {
    return (size == 0);
  }

  mapped_type& operator[](const key_type& key) {
    size_type hashedKey = getHash(key);
    size_type i = 0;
    while (i < tab[hashedKey].size()) {
        if(tab[hashedKey].at(i).first == key)
            return tab[hashedKey].at(i).second;
    }
    append(std::make_pair(key, mapped_type()));
    return tab[getHash(key)].front().second;
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
    if(tab[hashedKey].empty())
        return const_iterator(this);
    else
        return const_iterator(this, hashedKey, 0);
  }

  iterator find(const key_type& key) {
    size_type hashedKey = getHash(key);
    if(tab[hashedKey].empty())
        return iterator(this);
    else
        return iterator(this, hashedKey, 0);
  }

  void remove(const key_type& key) {
    (void)key;
    throw std::runtime_error("TODO");
  }

  void remove(const const_iterator& it) {
    (void)it;
    throw std::runtime_error("TODO");
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
        return iterator(this);
    size_type i = 0;
    while(i < MAX_SIZE) {
        if(tab[i].empty())
            i++;
        else
            break;
    }
    return iterator(this, i, 0);
  }

  iterator end() {
    return iterator(this);
  }

  const_iterator cbegin() const {
    if(isEmpty())
        return const_iterator(this);
    size_type i = 0;
    while(i < MAX_SIZE) {
        if(tab[i].empty())
            i++;
        else
            break;
    }
    return const_iterator(this, i, 0);
  }

  const_iterator cend() const {
    return const_iterator(this);
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

private:
  const HashMap *map;
  size_type tabIndex, innerIndex;

public:

  explicit ConstIterator(const HashMap *map, size_type tabIndex = MAX_SIZE, size_type innerIndex = MAX_SIZE) {
    this->map = map;
    this->tabIndex = tabIndex;
    this->innerIndex = innerIndex;
  }

  ConstIterator(const ConstIterator& other) {
    this->map = other.map;
    this->tabIndex = other.tabIndex;
    this->innerIndex = other.innerIndex;
  }

  ConstIterator& operator++() {
    if(map->isEmpty())
        throw std::out_of_range("Cannot increment while map is empty");
    if(tabIndex == MAX_SIZE)
        throw std::out_of_range("Trying to increment end");
    if(map->tab[tabIndex].size() > innerIndex + 1)
        innerIndex++;
    else {
        tabIndex++;
        while(tabIndex < MAX_SIZE) {
            if(map->tab[tabIndex].empty())
                tabIndex++;
            else
                break;
        }
        if(tabIndex == MAX_SIZE)
            innerIndex = MAX_SIZE;
    }
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
    if(tabIndex == MAX_SIZE) {
        tabIndex--;
        innerIndex = 0;
        while(tabIndex > 0) { //another duplication of code
            if(map->tab[tabIndex].empty())
                tabIndex--;
            else
                break;
        }
        return *this;
    }
    if(innerIndex > 0)
        innerIndex--;
    else {
        tabIndex--;
        while(tabIndex > 0) {
            if(map->tab[tabIndex].empty())
                tabIndex--;
            else
                break;
        }
        if(tabIndex == MAX_SIZE)
            innerIndex = MAX_SIZE;
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
    return map->tab[tabIndex].at(innerIndex);
  }

  pointer operator->() const {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const {
    return ((other.innerIndex == this->innerIndex) && (other.tabIndex == this->tabIndex));
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

  explicit Iterator()
  {}

  Iterator(const HashMap *map, size_type tabIndex = MAX_SIZE, size_type innerIndex = MAX_SIZE) : ConstIterator(map, tabIndex, innerIndex){}

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
