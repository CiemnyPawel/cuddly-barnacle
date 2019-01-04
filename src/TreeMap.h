#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <stack>

namespace aisdi {

template<typename KeyType, typename ValueType>
class TreeMap {
public:
using key_type = KeyType;
using mapped_type = ValueType;
using value_type = std::pair<key_type, mapped_type>;
using size_type = std::size_t;
using reference = value_type &;
using const_reference = const value_type &;

class ConstIterator;

friend class ConstIterator;

using const_iterator = ConstIterator;

class Iterator;

using iterator = Iterator;

private:
struct Node {

        value_type value;
        Node *left{nullptr};
        Node *right{nullptr};
        int height{0};

        Node(value_type value) : value(value) {
        }

        key_type &getKey() {
                return value.first;
        }
};

using pNode = Node *;

pNode root{nullptr};
size_type size{0};

private:
int height(pNode t) {
        return (t == NULL ? 0 : t->height);
}

pNode insert(value_type inserted, pNode current) {
        key_type key = inserted.first;
        if (current == NULL) {
                current = new Node(inserted);
        }
        else
        if (key < current->getKey()) {
                current->left = insert(inserted, current->left);
                if (height(current->left) - height(current->right) == 2) {
                        if (key < current->left->getKey())
                                current = singleRightRotate(current);
                        else
                                current = doubleRightRotate(current);
                }
        }
        else if (key > current->getKey()) {
                current->right = insert(inserted, current->right);
                if (height(current->right) - height(current->left) == 2) {
                        if (key > current->right->getKey())
                                current = singleLeftRotate(current);
                        else
                                current = doubleLeftRotate(current);
                }
        }

        current->height = std::max(height(current->left), height(current->right)) + 1;
        return current;
}


pNode singleRightRotate(pNode &t) {
        pNode u = t->left;
        t->left = u->right;
        u->right = t;
        t->height = std::max(height(t->left), height(t->right)) + 1;
        u->height = std::max(height(u->left), t->height) + 1;
        return u;
}

pNode singleLeftRotate(pNode &t) {
        pNode u = t->right;
        t->right = u->left;
        u->left = t;
        t->height = std::max(height(t->left), height(t->right)) + 1;
        u->height = std::max(height(t->right), t->height) + 1;
        return u;
}

pNode doubleLeftRotate(pNode &t) {
        t->right = singleRightRotate(t->right);
        return singleLeftRotate(t);
}

pNode doubleRightRotate(pNode &t) {
        t->left = singleLeftRotate(t->left);
        return singleRightRotate(t);
}

void deleteSubtree(pNode p) {
        if (p == nullptr)
                return;
        deleteSubtree(p->left);
        deleteSubtree(p->right);
        delete p;
        return;
}

pNode remove(key_type x, pNode t) {
        pNode temp;

        // Element not found
        if (t == NULL)
                return NULL;

        // Searching for element
        else if (x < t->getKey())
                t->left = remove(x, t->left);
        else if (x > t->getKey())
                t->right = remove(x, t->right);

        //With 2 children
        else if (t->left && t->right) {
                auto it = ConstIterator(t->right, t->right);
                while (it.hasLeftSon()) {
                        it.moveToLeftSon();
                }
                t->value = *it;
                t->right = remove(it->first, t->right);
        }
        // With one or zero child
        else {
                temp = t;
                if (t->left == NULL)
                        t = t->right;
                else if (t->right == NULL)
                        t = t->left;
                delete temp;
        }
        if (t == NULL)
                return t;

        t->height = std::max(height(t->left), height(t->right)) + 1;

        // If node is unbalanced
        // If left node is deleted, right case
        if (height(t->left) - height(t->right) == 2) {
                // right right case
                if (height(t->left->left) - height(t->left->right) == 1)
                        return singleRightRotate(t);
                // right left case
                else
                        return doubleRightRotate(t);
        }
        // If right node is deleted, left case
        else if (height(t->right) - height(t->left) == 2) {
                // left left case
                if (height(t->right->right) - height(t->right->left) == 1)
                        return singleLeftRotate(t);
                // left right case
                else
                        return doubleLeftRotate(t);
        }
        return t;
}

public:
TreeMap() {
}

TreeMap(std::initializer_list<value_type> list) {
        for (auto item : list) {
                key_type index = item.first;
                mapped_type value = item.second;
                (*this)[index] = value;
        }
}

TreeMap(const TreeMap &other) {
        *this = other;
}

TreeMap(TreeMap &&other) {
        size = other.size;
        other.size = 0;
        root = other.root;
        other.root = nullptr;
}

TreeMap &operator=(const TreeMap &other) {

        if (*this == other)
                return *this;

        deleteSubtree(root);
        root = nullptr;
        size = 0;

        for (auto item : other) {
                key_type index = item.first;
                mapped_type value = item.second;
                (*this)[index] = value;
        }
        return *this;
}

TreeMap &operator=(TreeMap &&other) {
        if (*this == other)
                return *this;

        deleteSubtree(root);
        root = nullptr;
        size = 0;

        size = other.size;
        other.size = 0;
        root = other.root;
        other.root = nullptr;

        return *this;
}

~TreeMap() {
        deleteSubtree(root);
}

bool isEmpty() const {
        if (getSize() == 0)
                return 1;
        return 0;
}

mapped_type &operator[](const key_type &key) {
        auto found = find(key);
        if (found != end()) {
                return found->second;
        }
        else {
                size++;
                root = insert({key, mapped_type{}}, root);
                return find(key)->second;
        }

}

const mapped_type &valueOf(const key_type &key) const {
        auto found = find(key);
        if (found != end()) {
                return found->second;
        }
        else {
                throw std::out_of_range("Key not found in map.");
        }
}

mapped_type &valueOf(const key_type &key) {
        return const_cast<mapped_type & >((static_cast<const TreeMap *>(this))->valueOf(key));
}

const_iterator find(const key_type &key) const {
        if (root == nullptr)
                return cend();

        auto it = ConstIterator(root, root);
        while (it->first != key) {
                if (it->first > key) {

                        if (it.hasLeftSon())
                                it.moveToLeftSon();
                        else
                                return cend();

                }
                else if (it->first < key) {

                        if (it.hasRightSon())
                                it.moveToRightSon();
                        else
                                return cend();

                }
        }
        return it;
}

iterator find(const key_type &key) {
        return (static_cast<const TreeMap * >(this)->find(key));
}

void remove(const key_type &key) {
        if (find(key) != end()) {
                root = remove(key, root);
                size--;
        }
        else
                throw std::out_of_range("Removing missing element.");
        return;
}

void remove(const const_iterator &it) {
        remove(it->first);
        return;
}

size_type getSize() const {
        return size;
}

bool operator==(const TreeMap &other) const {
        if (size != other.size)
                return 0;
        for (const auto &item : other) {
                const auto it = find(item.first);
                if (it == end())
                        return 0;
                if (it->second != item.second)
                        return 0;
        }
        return 1;
}

bool operator!=(const TreeMap &other) const {
        return !(*this == other);
}

iterator begin() {
        return Iterator(cbegin());
}

iterator end() {
        return Iterator(cend());
}

const_iterator cbegin() const {
        if (isEmpty()) {
                return cend();
        }
        else {
                auto it = ConstIterator(root, root);
                while (it.hasLeftSon()) {
                        it.moveToLeftSon();
                }
                return it;
        }
}

const_iterator cend() const {
        return ConstIterator(nullptr, root);
}

const_iterator begin() const {
        return cbegin();
}

const_iterator end() const {
        return cend();
}
};

template<typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator {
public:
using reference = typename TreeMap::const_reference;
using iterator_category = std::bidirectional_iterator_tag;
using value_type = typename TreeMap::value_type;
using pointer = const typename TreeMap::value_type *;

friend class TreeMap;

private:
using pNode = typename TreeMap::pNode;
pNode actual;
pNode root;
std::stack<pNode> parents;

void moveToLeftSon() {
        parents.push(actual);
        actual = actual->left;
}

void moveToRightSon() {
        parents.push(actual);
        actual = actual->right;
}

void moveToParent() {
        actual = parents.top();
        parents.pop();
}

bool hasLeftSon() {
        if (actual->left != nullptr)
                return 1;
        return 0;
}

bool hasRightSon() {
        if (actual->right != nullptr)
                return 1;
        return 0;
}

bool hasRightParent() {
        if (parents.empty()) {
                return 0;
        }

        pNode &parent = parents.top();
        if (parent->left == actual) {
                return 1;
        }
        return 0;
}

bool hasLeftParent() {

        if (parents.empty()) {
                return 0;
        }

        pNode &parent = parents.top();
        if (parent->right == actual) {
                return 1;
        }
        return 0;
}

public:
explicit ConstIterator(pNode p, pNode root) : actual(p), root(root) {
}

ConstIterator(const ConstIterator &other) : actual(other.actual), root(other.root), parents(other.parents) {
}

ConstIterator &operator++() {
        if (root == nullptr)
                throw std::out_of_range("Incrementing iterator to empty map.");
        if (actual == nullptr)
                throw std::out_of_range("Incrementing end iterator.");

        if (hasRightSon()) {
                moveToRightSon();

                while (hasLeftSon()) {
                        moveToLeftSon();
                }
        }
        else {
                while (hasLeftParent()) {
                        moveToParent();
                }

                if (hasRightParent())
                        moveToParent();
                else
                        actual = nullptr;
        }
        return *this;
}

ConstIterator operator++(int) {
        auto result = *this;
        operator++();
        return result;
}

ConstIterator &operator--() {
        if (root == nullptr)
                throw std::out_of_range("Decrementing iterator to empty map.");
        if (actual == nullptr) {
                actual = root;
                while (hasRightSon()) {
                        moveToRightSon();
                }
        }
        else if (hasLeftSon()) {
                moveToLeftSon();

                while (hasRightSon()) {
                        moveToRightSon();
                }
        }
        else {
                while (hasRightParent()) {
                        moveToParent();
                }

                if (hasLeftParent())
                        moveToParent();
                else
                        throw std::out_of_range("Decrementing begin iterator.");
        }
        return *this;
}

ConstIterator operator--(int) {
        auto result = *this;
        operator--();
        return result;
}

reference operator*() const {
        if (actual == nullptr)
                throw std::out_of_range("Dereferencing end iterator");
        return actual->value;
}

pointer operator->() const {
        return &this->operator*();
}

bool operator==(const ConstIterator &other) const {
        if (actual == other.actual)
                return 1;
        else
                return 0;
}

bool operator!=(const ConstIterator &other) const {
        return !(*this == other);
}
};

template<typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator {
public:
using reference = typename TreeMap::reference;
using pointer = typename TreeMap::value_type *;

explicit Iterator() {
}

Iterator(const ConstIterator &other)
        : ConstIterator(other) {
}

Iterator &operator++() {
        ConstIterator::operator++();
        return *this;
}

Iterator operator++(int) {
        auto result = *this;
        ConstIterator::operator++();
        return result;
}

Iterator &operator--() {
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
