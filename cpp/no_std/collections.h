#pragma once

#include "memory.h"

namespace cpp
{

/// @brief Double-linked list of type T
template<typename T>
class List
{
private:
    /// @brief Double-linked node
    struct Node
    {
        Node* prev = this;
        Node* next = this;

        /// @brief  An empty node is linked to its self.
        Node() {}
        ~Node() { unlink(); }
        
        /// @brief Link node to the back of this.
        /// @param node node to link.
        /// @retval true if success, false otherwise.
        bool link_back(Node* node)
        {
            if (node == nullptr)
                return false;
            node->next = this->next;
            node->prev = this;
            this->next->prev = node;
            this->next = node;
            return true;
        }

        /// @brief Link node to the front of this.
        /// @param node node to link
        /// @retval true if success, false otherwise.
        bool link_front(Node* node)
        {
            if (node == nullptr)
                return false;
            node->prev = this->prev;
            node->next = this;
            this->prev->next = node;
            this->prev = node;
            return true;
        }

        /// @brief Unlink this node.
        void unlink()
        {
            this->next->prev = this->prev;
            this->prev->next = this->next;
            this->next = this->prev = this;
        }
    };
    struct NodeT : Node
    {
        T val;
        NodeT(T& t) : val(t) {}
        NodeT(T&& t) : val(move(t)) {}
    };
    Node node;

public:
    struct Iterator
    {
        friend class List;
    private:
        Node* node;
    public:
        Iterator(Node* n) : node(n) {}
        
        /// @brief Assignment operator
        Iterator& operator=(Iterator& i) { node = i.node; } 
        /// @brief Prefix increment operator
        Iterator& operator++() { node = node->next; return *this; }
        /// @brief Postfix increment operator
        Iterator operator++(int) { node = node->next; return *this; }
        /// @brief Not equal operator
        bool operator!=(const Iterator& b) { return node != b.node; }
        /// @brief Equal operator
        bool operator==(const Iterator& b) { return node == b.node; }
        /// @brief Indirection operator
        T& operator*() { return static_cast<NodeT*>(node)->val; }
        /// @brief Pointer-to-member operator
        T* operator->() { return &static_cast<NodeT*>(node)->val; }
    };

    List() {}
    ~List() { while (!empty()) erase(begin()); }
    /// @brief Push object to the back of list.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_back(T& t) { return node.link_front(new NodeT(t)); }
    /// @brief Push object to the back of list.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_back(T&& t) { return node.link_front(new NodeT(move(t))); }

    /// @brief Push object to the front of list.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_front(T& t) { return node.link_back(new NodeT(t)); }
    /// @brief Push object to the front of list.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_front(T&& t) { return node.link_back(new NodeT(move(t))); }

    /// @brief Erase the node pointed by iterator
    void erase(Iterator& i) { delete i.node; }
    /// @brief Erase the node pointed by iterator
    void erase(Iterator&& i) { delete i.node; }

    /// @brief Check if list is empty
    bool empty() { return begin() == end(); }

    /// @brief Get iterator that points to the first element of the list.
    /// @note Using iterator from an empty list may lead to undefined behaviour.
    Iterator begin() { return Iterator(node.next); }
    /// @brief Get iterator that points to one past the last element of the list.
    /// @note Using iterator from an empty list may lead to undefined behaviour.
    Iterator end() { return Iterator(&node); }
};

template<typename T>
class Queue : List<T>
{
public:
    using List<T>::begin;
    using List<T>::end;
    using List<T>::empty;

    
};

}