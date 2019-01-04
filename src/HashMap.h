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
size_t bucketsNumber = 50;
std::vector<std::list<std::pair<const KeyType, ValueType>* > > listVector;
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
        listVector.resize(bucketsNumber);
}

~HashMap()
{
        listVector.clear();
}

HashMap(size_t bucketsNR)
{
        bucketsNumber = bucketsNR;
        listVector.resize(bucketsNumber);
}

HashMap(std::initializer_list<value_type> list)
{
        std::hash<KeyType> itemHash;
        mapSize = 0;
        listVector.resize(bucketsNumber);
        for(auto i = list.begin(); i != list.end(); i++)
        {
                value_type* temporary = new value_type(i->first, i->second);
                listVector[itemHash(i->first)%bucketsNumber].push_back(temporary);
                mapSize++;
        }
}

HashMap(const HashMap& other)
{
        listVector = other.listVector;
        mapSize = other.mapSize;
        bucketsNumber = other.bucketsNumber;
}

HashMap(HashMap&& other)
{
        listVector = std::move(other.listVector);
        mapSize = std::move(other.mapSize);
        bucketsNumber = std::move(other.bucketsNumber);
}

HashMap& operator=(const HashMap& other)
{
        if(this == &other)
                return *this;
        bucketsNumber = other.bucketsNumber;
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
        bucketsNumber = std::move(other.bucketsNumber);
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
                        if((*(*it)).first == key)
                                return (*(*it)).second;
                }
        }
        if (isTooSmall())
        {
                makeBigger();
                listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        }

        value_type* temporary = new value_type{key, mapped_type{}};
        listVector[listNumber].push_back(temporary);
        mapSize++;
        return (*(*listVector[listNumber].begin())).second;
}

const mapped_type& valueOf(const key_type& key) const
{
        size_type listNumber = (size_t)(std::hash<KeyType >{} (key) % bucketsNumber);
        if(listVector[listNumber].empty() == 0)
        {
                for(auto it = listVector[listNumber].begin(); it != listVector[listNumber].end(); it++)
                {
                        if((*(*it)).first == key)
                                return (*(*it)).second;
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
                        if((*(*it)).first == key)
                                return (*(*it)).second;
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
                        if((*(*it)).first == key)
                                return ConstIterator(this, listNumber, it);
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
                        if((*(*it)).first == key)
                                return Iterator(ConstIterator(this, listNumber, it));
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
                        if((*(*it)).first == key)
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
        if(listVector[it.numberOfList].empty() == 0 and it.iteratorInCurrentList != listVector[it.numberOfList].end())
        {
                listVector.at(it.numberOfList).erase(it.iteratorInCurrentList);
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
        if(mapSize != other.mapSize)
                return false;
        for(auto it = other.begin(); it != other.end(); it++)
        {
                auto temporary = find(it->first);
                if(temporary == end())
                        return 0;
                if(valueOf(it->first) != other.valueOf(it->first))
                        return 0;
        }
        return 1;
        /*
            if(listVector == other.listVector)
                return true;
            else
                return false;*/
}

bool operator!=(const HashMap& other) const
{
        return !(*this == other);
}

iterator begin()
{
        if(mapSize == 0)
                return cend();
        for(auto it = listVector.begin(); it != listVector.end(); it++)
                if((*it).empty() == 0)
                        return Iterator(ConstIterator(this, std::distance(listVector.begin(), it), (*it).begin()));
}

iterator end()
{
        return Iterator(ConstIterator(this, bucketsNumber - 1, listVector[bucketsNumber - 1].end()));
}

const_iterator cbegin() const
{
        if(mapSize == 0)
                return cend();
        for(auto it = listVector.begin(); it != listVector.end(); it++)
                if((*it).empty() == 0)
                        return ConstIterator(this, std::distance(listVector.begin(), it), (*it).begin());
}

const_iterator cend() const
{
        return ConstIterator(this, bucketsNumber - 1, listVector[bucketsNumber - 1].end());
}

const_iterator begin() const
{
        return cbegin();
}

const_iterator end() const
{
        return cend();
}

private:
bool isTooSmall()
{
        float loadFactor = (mapSize) / (bucketsNumber*100);
        if (loadFactor > 0.75)
                return 1;
        return 0;
}

void makeBigger()
{
        HashMap<key_type, mapped_type> bigger(bucketsNumber * 2);
        for (auto it = listVector.begin(); it != listVector.end(); it++)
        {
                for(auto i = (*it).begin(); i != (*it).end(); i++)
                        bigger[(**i).first] = (**i).second;
        }
        *this = std::move(bigger);

        return;
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
using listIterator = typename std::list<std::pair<const KeyType,ValueType>* >::const_iterator;

const HashMap *ptrToHashMap;
size_t numberOfList;
listIterator iteratorInCurrentList;

explicit ConstIterator(const HashMap *ptr, size_t listNum, listIterator it)
{
        ptrToHashMap = ptr;
        numberOfList = (size_t)listNum;
        iteratorInCurrentList = it;
}

ConstIterator(const ConstIterator &other)
{
        ptrToHashMap = other.ptrToHashMap;
        numberOfList = other.numberOfList;
        iteratorInCurrentList = other.iteratorInCurrentList;
}

ConstIterator& operator++()
{
        if(*this == ptrToHashMap->end())
                throw std::out_of_range("Incrementing map's end.");
        iteratorInCurrentList++;
        while(iteratorInCurrentList == ptrToHashMap->listVector[numberOfList].end())
        {
                numberOfList++;
                if(numberOfList == ptrToHashMap->bucketsNumber)
                {
                        *this = ptrToHashMap->end();
                        return *this;
                }
                iteratorInCurrentList = ptrToHashMap->listVector[numberOfList].begin();
        }
        return *this;
}

ConstIterator operator++(int)
{
        auto ToReturn = *this;
        ++(*this);
        return ToReturn;
}

ConstIterator& operator--()
{
        if(*this == ptrToHashMap->begin())
                throw std::out_of_range("Decrementing map's start.");

        while(ptrToHashMap->listVector[numberOfList].begin() == ptrToHashMap->listVector[numberOfList].end())
        {
                numberOfList--;
                iteratorInCurrentList = ptrToHashMap->listVector[numberOfList].end();
        }
        iteratorInCurrentList--;
        return *this;
}

ConstIterator operator--(int)
{
        auto ToReturn = *this;
        --(*this);
        return ToReturn;
}

reference operator*() const
{
        if(*this == ptrToHashMap->end())
                throw std::out_of_range("Dereferencing map's end.");
        return **iteratorInCurrentList;
}

pointer operator->() const
{
        return &this->operator*();
}

bool operator==(const ConstIterator& other) const
{
        return iteratorInCurrentList == other.iteratorInCurrentList;
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
