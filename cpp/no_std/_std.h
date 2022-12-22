#pragma once

namespace _std
{
/// @brief Convert object to rvalue
template<typename T>
inline T&& move(T& arg) { return static_cast<T&&>(arg); }
/// @brief Convert object to rvalue
template<typename T>
inline T&& move(T&& arg) { return static_cast<T&&>(arg); }

/// @brief Swap objects' value
template<typename T>
void swap(T& a, T& b) { auto x=_std::move(a); a=_std::move(b); b=_std::move(x); }

/// @brief Unique pointer
template<typename T>
class unique_ptr
{
private:
    T* _obj;
public:
    unique_ptr() : _obj(nullptr) {};
    unique_ptr(T* obj) : _obj(obj) {}
    unique_ptr(unique_ptr&& ptr) : _obj(ptr._obj) { ptr._obj = nullptr; }
    ~unique_ptr() { if (_obj != nullptr) delete _obj; }

    /// @brief Assignment operator
    unique_ptr& operator=(unique_ptr&& b) { _obj = b._obj; b._obj = nullptr; return *this; } 
    /// @brief Not equal operator
    bool operator!=(const unique_ptr& b) const { return _obj != b._obj; }
    bool operator!=(void* b) const { return _obj != b; }
    /// @brief Equal operator
    bool operator==(const unique_ptr& b) const { return _obj == b._obj; }
    bool operator==(void* b) const { return _obj == b; }
    /// @brief Logical NOT operator
    bool operator!() const { return _obj == nullptr; }
    /// @brief Bool operator
    operator bool() const { return _obj != nullptr; }
    /// @brief Indirection operator
    T& operator*() const { return *_obj; }
    /// @brief Pointer-to-member operator
    T* operator->() const { return _obj; }

    /// @brief Get pointer to the managed object.
    T* get() { return _obj; }

    /// @brief Release the ownership of the managed object.
    /// @return Pointer to the managed object.
    T* release() { auto ret = _obj; _obj = nullptr; return ret; }
};

/// @brief Constructs an object of type T and wraps it in a unique_ptr.
/// @param args list of arguments with which an instance of T will be constructed.
/// @retval unique_ptr of an instance of type T.
template<typename T, typename ...Args>
unique_ptr<T> make_unique(Args&& ...args) { return new T(args...); }


/// @brief Double-linked list of type T
template<typename T>
class list
{
private:
    /// @brief Double-linked node
    struct node
    {
        node* _prev = this;
        node* _next = this;

        /// @brief  An empty node is linked to its self.
        node() {}
        ~node() { unlink(); }
        
        /// @brief Link node to the back of this.
        /// @param n node to link.
        /// @retval true if success, false otherwise.
        bool link_back(node* n)
        {
            if (n == nullptr)
                return false;
            n->_next = this->_next;
            n->_prev = this;
            this->_next->_prev = n;
            this->_next = n;
            return true;
        }

        /// @brief Link node to the front of this.
        /// @param node node to link
        /// @retval true if success, false otherwise.
        bool link_front(node* n)
        {
            if (n == nullptr)
                return false;
            n->_prev = this->_prev;
            n->_next = this;
            this->_prev->_next = n;
            this->_prev = n;
            return true;
        }

        /// @brief Unlink this node.
        void unlink()
        {
            this->_next->_prev = this->_prev;
            this->_prev->_next = this->_next;
            this->_next = this->_prev = this;
        }
    };
    struct node_t : node
    {
        T _val;
        node_t(T& val) : _val(val) {}
        node_t(T&& val) : _val(move(val)) {}
    };
    node _node;

public:
    struct iterator
    {
        friend class list;
    private:
        node* _node;
    public:
        iterator(const node* n) : _node(decltype(_node)(n)) {}
        
        /// @brief Assignment operator
        iterator& operator=(const iterator& i) { _node = i._node; return *this;} 
        /// @brief Prefix increment operator
        iterator& operator++() { _node = _node->_next; return *this; }
        /// @brief Postfix increment operator
        iterator operator++(int) { _node = _node->_next; return *this; }
        /// @brief Prefix decrement operator
        iterator& operator--() { _node = _node->_prev; return *this; }
        /// @brief Postfix decrement operator
        iterator operator--(int) { _node = _node->_prev; return *this; }
        /// @brief Not equal operator
        bool operator!=(const iterator& b) const { return _node != b._node; }
        /// @brief Equal operator
        bool operator==(const iterator& b) const { return _node == b._node; }
        /// @brief Indirection operator
        T& operator*() const { return static_cast<node_t*>(_node)->_val; }
        /// @brief Pointer-to-member operator
        T* operator->() const { return &static_cast<node_t*>(_node)->_val; }
    };

    list() {}
    ~list() { clear(); }

    /// @brief Push object to the back of container.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_back(T& t) { return _node.link_front(new node_t(t)); }
    /// @brief Push object to the back of container.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_back(T&& t) { return _node.link_front(new node_t(_std::move(t))); }

    /// @brief Push object to the front of container.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_front(T& t) { return _node.link_back(new node_t(t)); }
    /// @brief Push object to the front of container.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push_front(T&& t) { return _node.link_back(new node_t(_std::move(t))); }

    /// @brief Remove last element of the container.
    /// @note Calling pop_back on an empty container results in undefined behavior.
    void pop_back() { delete static_cast<node_t*>(_node._prev); }

    /// @brief Remove first element of the container.
    /// @note Calling pop_front on an empty container results in undefined behavior.
    void pop_front() { delete static_cast<node_t*>(_node._next); }

    /// @brief Erase the specified elements.
    /// @return Iterator following the last removed element.
    /// @note Erase an invalid iterator may lead to undefined behaviour.
    iterator erase(iterator& i) { auto next = i._node->_next; delete static_cast<node_t*>(i._node); return next;}
    /// @brief Erase the specified elements.
    /// @return Iterator following the last removed element.
    /// @note Erase an invalid iterator may lead to undefined behaviour.
    iterator erase(iterator&& i) { auto next = i._node->_next; delete static_cast<node_t*>(i._node); return next;}

    /// @brief Erase all elements from the container.
    void clear() { while (!empty()) delete static_cast<node_t*>(_node._next); }

    /// @brief Check if list is empty
    bool empty() const { return _node._next == &_node; }

    /// @brief Get iterator that points to the first element.
    /// @note Using iterator from an empty container may lead to undefined behaviour.
    iterator begin() const { return _node._next; }
    /// @brief Get iterator that points to one after the last element.
    /// @note Using iterator from an empty container may lead to undefined behaviour.
    iterator end() const { return &_node; }

    /// @brief Access the first element.
    /// @note Accessing an empty container results in undefined behavior.
    T& front() const { return static_cast<node_t*>(_node._next)->_val; }
    /// @brief Access last element.
    /// @note Accessing an empty container results in undefined behavior.
    T& back() const { return static_cast<node_t*>(_node._prev)->_val; }
};

/// @brief Queue of type T
template<typename T>
class queue : _std::list<T>
{
public:
    using _std::list<T>::empty;
    using _std::list<T>::front;
    using _std::list<T>::back;

    /// @brief Push object to the back of container.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push(T& t) { return _std::list<T>::push_back(t); }
    /// @brief Push object to the back of container.
    /// @param t Reference to the object.
    /// @return true if success, false otherwise.
    bool push(T&& t) { return _std::list<T>::push_back(move(t)); }

    /// @brief Remove first element of the container.
    /// @note Calling pop on an empty container results in undefined behavior.
    void pop() { return _std::list<T>::pop_front(); }
};

}