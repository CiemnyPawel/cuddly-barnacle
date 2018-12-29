#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>
#include <array>
#include <functional>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
private:
std::array<std::list<std::pair<const KeyType,ValueType> >, 50> listArray;
size_t mapSize = 0;
static const size_t bucketsNumber = 50;
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
        (void)list; // disables "unused argument" warning, can be removed when method is implemented.
        throw std::runtime_error("TODO");
        mapSize = 0;
        for(auto i = list.begin(); i < list.end(); i++)
        {
        }
}

HashMap(const HashMap& other)
{
        (void)other;
        throw std::runtime_error("TODO");
}

HashMap(HashMap&& other)
{
        (void)other;
        throw std::runtime_error("TODO");
}

HashMap& operator=(const HashMap& other)
{
        (void)other;
        throw std::runtime_error("TODO");
}

HashMap& operator=(HashMap&& other)
{
        (void)other;
        throw std::runtime_error("TODO");
}

bool isEmpty() const
{
        throw std::runtime_error("TODO");
}

mapped_type& operator[](const key_type& key) // TODO
{
        (void)key;
        throw std::runtime_error("TODO");
        std::hash<key_type> itemHash;
        size_type listNumber = itemHash(key)%bucketsNumber;
        if(listArray[listNumber].empty() == 0)
        {
                for(auto i = listArray[listNumber].begin(); i < listArray[listNumber].end(); i++)
                {
                        if((*i).first == key)
                                return (*i).second;
                }
                std::pair<key_type, mapped_type> temporary{key, mapped_type{}};
                temporary.first = key;
                listArray[listNumber].push_back(temporary);
                return temporary.second;
        }
        else
        {
                std::pair<key_type, mapped_type> temporary{key, mapped_type{}};
                temporary.first = key;
                listArray[listNumber].push_back(temporary);
                return temporary.second;
        }
}

const mapped_type& valueOf(const key_type& key) const
{
        (void)key;
        throw std::runtime_error("TODO");
}

mapped_type& valueOf(const key_type& key)
{
        (void)key;
        throw std::runtime_error("TODO");
}

const_iterator find(const key_type& key) const
{
        (void)key;
        throw std::runtime_error("TODO");
}

iterator find(const key_type& key)
{
        (void)key;
        throw std::runtime_error("TODO");
}

void remove(const key_type& key)
{
        (void)key;
        throw std::runtime_error("TODO");
}

void remove(const const_iterator& it)
{
        (void)it;
        throw std::runtime_error("TODO");
}

size_type getSize() const
{
        throw std::runtime_error("TODO");
}

bool operator==(const HashMap& other) const
{
        (void)other;
        throw std::runtime_error("TODO");
}

bool operator!=(const HashMap& other) const
{
        return !(*this == other);
}

iterator begin()
{
        throw std::runtime_error("TODO");
}

iterator end()
{
        throw std::runtime_error("TODO");
}

const_iterator cbegin() const
{
        throw std::runtime_error("TODO");
}

const_iterator cend() const
{
        throw std::runtime_error("TODO");
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
