#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>
#include <array>
#include <functional>
#include <string>
#include <vector>


namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
private:
static const size_t bucketsNumber = 50;
std::vector<std::list<std::pair<const KeyType, ValueType> > > listVector;
size_t mapSize = 0;
public:
using key_type = KeyType;
using mapped_type = ValueType;
using value_type = std::pair<const key_type, mapped_type>;
using size_type = std::size_t;
using reference = value_type&;
using const_reference = const value_type &;

class ConstIterator;
class Iterator;
using iterator = Iterator;
using const_iterator = ConstIterator;

HashMap()
{
        mapSize = 0;
}

HashMap(std::initializer_list<value_type> list)
{
        std::hash<KeyType> itemHash;
        mapSize = 0;
        for(auto i = list.begin(); i != list.end(); i++)
        {
                listVector[itemHash(i->first)%bucketsNumber].push_back(*i);
                mapSize++;
        }
}

HashMap(const HashMap& other)
{
        listVector = other.listVector;
        mapSize = other.mapSize;
}

HashMap(HashMap&& other)
{
        listVector = std::move(other.listVector);
        mapSize = std::move(other.mapSize);
}

HashMap& operator=(const HashMap& other)
{
        if(this == &other)
                return *this;
        listVector = other.listVector;
        mapSize = other.mapSize;
        return *this;
}

HashMap& operator=(HashMap&& other)
{
        if(this == &other)
                return *this;
        listVector = std::move(other.listVector);
        mapSize = std::move(other.mapSize);
        return *this;
}

bool isEmpty() const
{
        for(auto it = listVector.begin(); it != listVector.end(); it++)
        {
                if((*it).empty() == 0)
                        return false;
        }
        return true;
}

mapped_type& operator[](const key_type& key)
{
        size_type listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        if(listVector[listNumber].empty() == 0)
        {
                for(auto it = listVector[listNumber].begin(); it != listVector[listNumber].end(); it++)
                {
                        if((*it).first == key)
                                return (*it).second;
                }
        }
        std::pair<key_type, mapped_type> temporary{key, mapped_type{}};
        temporary.first = key;
        listVector[listNumber].push_back(temporary);
        return (*listVector[listNumber].begin()).second;
}

const mapped_type& valueOf(const key_type& key) const
{
        size_type listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        if(listVector[listNumber].empty() == 0)
        {
                for(auto it = listVector[listNumber].begin(); it != listVector[listNumber].end(); it++)
                {
                        if((*it).first == key)
                                return (*it).second;
                }
        }
        throw std::out_of_range("Key doesn't exist - valueOf");
}

mapped_type& valueOf(const key_type& key)
{
        size_type listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        if(listVector[listNumber].empty() == 0)
        {
                for(auto it = listVector[listNumber].begin(); it != listVector[listNumber].end(); it++)
                {
                        if((*it).first == key)
                                return (*it).second;
                }
        }
        throw std::out_of_range("Key doesn't exist - valueOf");
}

const_iterator find(const key_type& key) const
{
        size_type listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        if(listVector[listNumber].empty() == 0)
        {
                for(auto it = listVector[listNumber].begin(); it != listVector[listNumber].end(); it++)
                {
                        if((*it).first == key)
                                return ConstIterator{this, &listVector[listNumber], it};
                }
        }
        return cend();
}

iterator find(const key_type& key)
{
        size_type listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        if(listVector[listNumber].empty() == 0)
        {
                for(auto it = listVector[listNumber].begin(); it != listVector[listNumber].end(); it++)
                {
                        if((*it).first == key)
                                return Iterator{this, &listVector[listNumber], it};
                }
        }
        return end();
}

void remove(const key_type& key)
{
        size_type listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        if(listVector[listNumber].empty() == 0)
        {
                for(auto it = listVector[listNumber].begin(); it != listVector[listNumber].end(); it++)
                {
                        if((*it).first == key)
                        {
                                listVector[listNumber].erase(it);
                                mapSize--;
                                return;
                        }
                }
        }
        throw std::out_of_range("Key doesn't exist - remove");
}

void remove(const const_iterator& it)
{
        if(it.ptrToList.empty() == 0 and it.iteratorInList != it.ptrToList.end())
        {
                it.ptrToList.erase(it.iteratorInList);
                mapSize--;
                return;
        }
        throw std::out_of_range("Key doesn't exist - remove");
}

size_type getSize() const
{
        return mapSize;
}

bool operator==(const HashMap& other) const
{
        (void)other;
        throw std::runtime_error("TODO");
        if(listVector == other.listVector)
                return true;
        else
                return false;
}

bool operator!=(const HashMap& other) const
{
        return !(*this == other);
}

iterator begin()
{
        for(auto it = listVector.begin(); it != listVector.end(); it++)
                if((*it).empty() == 0)
                        return Iterator(this, &*it, it);
}

iterator end()
{
        return Iterator(this, &listVector[bucketsNumber+1], listVector[bucketsNumber+1].begin());
}

const_iterator cbegin() const
{
        for(auto it = listVector.begin(); it != listVector.end(); it++)
                if((*it).empty() == 0)
                        return ConstIterator(this, &*it, it);
}

const_iterator cend() const
{
        return ConstIterator(this, &listVector[bucketsNumber+1], listVector[bucketsNumber+1].begin());
}

const_iterator begin() const
{
        return cbegin();
}

const_iterator end() const
{
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

const HashMap &ptrToHashMap;
std::list<std::pair<const KeyType,ValueType> > &ptrToList;
typename std::list<std::pair<const KeyType,ValueType> >::iterator iteratorInList;

explicit ConstIterator()
{
}

ConstIterator(const ConstIterator &other)
{
        (void)other;
        throw std::runtime_error("TODO");
}

ConstIterator& operator++()
{
        throw std::runtime_error("TODO");
}

ConstIterator operator++(int)
{
        throw std::runtime_error("TODO");
}

ConstIterator& operator--()
{
        throw std::runtime_error("TODO");
}

ConstIterator operator--(int)
{
        throw std::runtime_error("TODO");
}

reference operator*() const
{
        throw std::runtime_error("TODO");
}

pointer operator->() const
{
        return &this->operator*();
}

bool operator==(const ConstIterator& other) const
{
        (void)other;
        throw std::runtime_error("TODO");
}

bool operator!=(const ConstIterator& other) const
{
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
{
}

Iterator(const ConstIterator &other)
        : ConstIterator(other)
{
}

Iterator& operator++()
{
        ConstIterator::operator++();
        return *this;
}

Iterator operator++(int)
{
        auto result = *this;
        ConstIterator::operator++();
        return result;
}

Iterator& operator--()
{
        ConstIterator::operator--();
        return *this;
}

Iterator operator--(int)
{
        auto result = *this;
        ConstIterator::operator--();
        return result;
}

pointer operator->() const
{
        return &this->operator*();
}

reference operator*() const
{
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
}
};

}


#endif /* AISDI_MAPS_HASHMAP_H */
