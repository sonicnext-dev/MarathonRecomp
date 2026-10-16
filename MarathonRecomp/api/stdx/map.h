#pragma once

#include <kernel/heap.h>

namespace stdx
{
    template <typename Key, typename T, typename Compare = std::less<Key>>
    class map
    {
    private:
        struct Node
        {
            xpointer<Node> _Left;
            xpointer<Node> _Parent;
            xpointer<Node> _Right;
            std::pair<const Key, T> _Myval;
            char _Color;
            bool _Isnil;

            Node(const Key& key, const T& value, xpointer<Node> parent = nullptr, char color = 0)
                : _Left(nullptr), _Parent(parent), _Right(nullptr), _Myval(key, value),
                _Color(color), _Isnil(false) 
            {

            }

            Node(bool isnil = false)
                : _Left(nullptr), _Parent(nullptr), _Right(nullptr), _Myval(),
                _Color(1), _Isnil(isnil) 
            {

            }
        };

        be<uint32_t> _Myproxy;
        xpointer<Node> _Myhead;
        be<uint32_t> _Mysize;

        xpointer<Node> allocate_node(const Key& key, const T& value, xpointer<Node> parent = nullptr, char color = 0)
        {
            void* memory = g_userHeap.Alloc(sizeof(Node));
            return xpointer<Node>(new (memory) Node(key, value, parent, color));
        }

        xpointer<Node> allocate_node(bool isnil = false)
        {
            void* memory = g_userHeap.Alloc(sizeof(Node));
            return xpointer<Node>(new (memory) Node(isnil));
        }

        void deallocate_node(xpointer<Node> node)
        {
            if (node && node.get())
            {
                node->~Node();
                g_userHeap.Free(node.get());
            }
        }

        bool compare_keys(const Key& a, const Key& b) const
        {
            return Compare()(a, b);
        }

        xpointer<Node> insert_node(const Key& key, const T& value)
        {
            if (_Myhead->_Left == _Myhead)
            {
                _Mysize = _Mysize + 1;
                xpointer<Node> newNode = allocate_node(key, value, _Myhead, 1);
                _Myhead->_Left = newNode;
                _Myhead->_Parent = newNode;
                _Myhead->_Right = newNode;
                return newNode;
            }

            xpointer<Node> current = _Myhead->_Parent;
            xpointer<Node> parent = _Myhead;

            while (current != _Myhead)
            {
                parent = current;
                if (compare_keys(key, current->_Myval.first))
                    current = current->_Left;
                else if (compare_keys(current->_Myval.first, key))
                    current = current->_Right;
                else
                {
                    current->_Myval.second = value;
                    return current;
                }
            }

            _Mysize = _Mysize + 1;
            xpointer<Node> newNode = allocate_node(key, value, parent, 0);

            if (compare_keys(key, parent->_Myval.first))
                parent->_Left = newNode;
            else
                parent->_Right = newNode;

            if (compare_keys(key, _Myhead->_Left->_Myval.first))
                _Myhead->_Left = newNode;
            if (compare_keys(_Myhead->_Right->_Myval.first, key))
                _Myhead->_Right = newNode;

            return newNode;
        }

        xpointer<Node> find_node(const Key& key) const
        {
            xpointer<Node> current = _Myhead->_Parent;
            while (current != _Myhead)
            {
                if (compare_keys(key, current->_Myval.first))
                    current = current->_Left;
                else if (compare_keys(current->_Myval.first, key))
                    current = current->_Right;
                else
                    return current;
            }
            return _Myhead;
        }

        void clear_tree(xpointer<Node> node)
        {
            if (node != _Myhead && node != nullptr)
            {
                clear_tree(node->_Left);
                clear_tree(node->_Right);
                deallocate_node(node);
            }
        }

        xpointer<Node> find_min(xpointer<Node> node) const
        {
            while (node->_Left != _Myhead)
                node = node->_Left;
            return node;
        }

        xpointer<Node> find_max(xpointer<Node> node) const
        {
            while (node->_Right != _Myhead)
                node = node->_Right;
            return node;
        }

    public:
        class iterator
        {
        private:
            xpointer<Node> _Ptr;
            xpointer<Node> _Myhead;

            xpointer<Node> find_min(xpointer<Node> node) const
            {
                while (node->_Left != _Myhead)
                    node = node->_Left;
                return node;
            }

        public:
            iterator(xpointer<Node> node = nullptr, xpointer<Node> head = nullptr)
                : _Ptr(node), _Myhead(head) 
            {

            }

            std::pair<const Key, T>& operator*() const { return _Ptr->_Myval; }
            std::pair<const Key, T>* operator->() const { return &_Ptr->_Myval; }

            iterator& operator++()
            {
                if (_Ptr == _Myhead) return *this;

                if (_Ptr->_Right != _Myhead)
                {
                    _Ptr = find_min(_Ptr->_Right);
                }
                else
                {
                    xpointer<Node> parent = _Ptr->_Parent;
                    while (parent != _Myhead && _Ptr == parent->_Right)
                    {
                        _Ptr = parent;
                        parent = parent->_Parent;
                    }
                    _Ptr = parent;
                }
                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const
            { 
                return _Ptr == other._Ptr; 
            }

            bool operator!=(const iterator& other) const 
            {
                return _Ptr != other._Ptr; 
            }
        };

        map() : _Mysize(0)
        {
            _Myhead = allocate_node(true);
            _Myhead->_Left = _Myhead;
            _Myhead->_Parent = _Myhead;
            _Myhead->_Right = _Myhead;
        }

        ~map()
        {
            clear();
            deallocate_node(_Myhead);
        }

        iterator begin()
        {
            return iterator(_Myhead->_Left, _Myhead);
        }

        iterator end()
        {
            return iterator(_Myhead, _Myhead);
        }

        size_t size() const 
        {
            return _Mysize; 
        }

        bool empty() const
        {
            return _Mysize == 0; 
        }

        T& operator[](const Key& key)
        {
            xpointer<Node> node = find_node(key);
            if (node == _Myhead)
                node = insert_node(key, T());
            return node->_Myval.second;
        }

        iterator find(const Key& key)
        {
            return iterator(find_node(key), _Myhead);
        }

        void clear()
        {
            clear_tree(_Myhead->_Parent);
            _Myhead->_Left = _Myhead;
            _Myhead->_Parent = _Myhead;
            _Myhead->_Right = _Myhead;
            _Mysize = 0;
        }

        std::pair<iterator, bool> insert(const std::pair<const Key, T>& value)
        {
            xpointer<Node> node = find_node(value.first);
            if (node != _Myhead)
                return std::make_pair(iterator(node, _Myhead), false);

            node = insert_node(value.first, value.second);
            return std::make_pair(iterator(node, _Myhead), true);
        }
    };
}
