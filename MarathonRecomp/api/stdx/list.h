#pragma once

#include <kernel/heap.h>

namespace stdx
{
    template <typename Value>
    class list
    {
    private:
        struct _List_node
        {
            _List_node(const Value& val)
                : _Next(nullptr), _Prev(nullptr), _Myval(val)
            {
            }

            _List_node(Value&& val)
                : _Next(nullptr), _Prev(nullptr), _Myval(std::move(val))
            {
            }

            xpointer<_List_node> _Next;
            xpointer<_List_node> _Prev;
            Value _Myval;
        };

        be<uint32_t> _MyProxy;
        xpointer<_List_node> _Myhead;
        be<uint32_t> _Mysize;

        xpointer<_List_node> allocate_node(const Value& value)
        {
            void* memory = g_userHeap.Alloc(sizeof(_List_node));
            return xpointer<_List_node>(new (memory) _List_node(value));
        }

        xpointer<_List_node> allocate_node(Value&& value)
        {
            void* memory = g_userHeap.Alloc(sizeof(_List_node));
            return xpointer<_List_node>(new (memory) _List_node(std::move(value)));
        }

        void deallocate_node(xpointer<_List_node> node)
        {
            if (node && node.get())
            {
                node->~_List_node();
                g_userHeap.Free(node.get());
            }
        }

        void link_nodes(xpointer<_List_node> prev, xpointer<_List_node> next)
        {
            if (prev) prev->_Next = next;
            if (next) next->_Prev = prev;
        }

    public:
        class iterator
        {
        private:
            xpointer<_List_node> _Ptr;

        public:
            iterator(xpointer<_List_node> node = nullptr) : _Ptr(node) {}

            Value& operator*() const { return _Ptr->_Myval; }
            Value* operator->() const { return &_Ptr->_Myval; }

            iterator& operator++()
            {
                if (_Ptr) _Ptr = _Ptr->_Next;
                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            iterator& operator--()
            {
                if (_Ptr) _Ptr = _Ptr->_Prev;
                return *this;
            }

            iterator operator--(int)
            {
                iterator tmp = *this;
                --(*this);
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

            xpointer<_List_node> _Mynode() const { return _Ptr; }
        };

        list() : _Mysize(0)
        {
            // Create sentinel node
            _Myhead = allocate_node(Value());
            _Myhead->_Next = _Myhead;
            _Myhead->_Prev = _Myhead;
        }

        ~list()
        {
            clear();
            deallocate_node(_Myhead);
        }

        iterator begin()
        {
            return iterator(_Myhead->_Next);
        }

        iterator end()
        {
            return iterator(_Myhead);
        }

        size_t size() const
        {
            return _Mysize;
        }

        bool empty() const
        {
            return _Mysize == 0;
        }

        Value& front()
        {
            return _Myhead->_Next->_Myval;
        }

        Value& back()
        {
            return _Myhead->_Prev->_Myval;
        }

        void push_front(const Value& value)
        {
            insert_node(_Myhead->_Next, value);
        }

        void push_front(Value&& value)
        {
            insert_node(_Myhead->_Next, std::move(value));
        }

        void push_back(const Value& value)
        {
            insert_node(_Myhead, value);
        }

        void push_back(Value&& value)
        {
            insert_node(_Myhead, std::move(value));
        }

        void pop_front()
        {
            if (!empty())
                erase_node(_Myhead->_Next);
        }

        void pop_back()
        {
            if (!empty())
                erase_node(_Myhead->_Prev);
        }

        iterator insert(iterator position, const Value& value)
        {
            return iterator(insert_node(position._Mynode(), value));
        }

        iterator insert(iterator position, Value&& value)
        {
            return iterator(insert_node(position._Mynode(), std::move(value)));
        }

        iterator erase(iterator position)
        {
            if (position == end()) return end();
            iterator next = position;
            ++next;
            erase_node(position._Mynode());
            return next;
        }

        void clear()
        {
            while (!empty())
                pop_front();
        }

    private:
        xpointer<_List_node> insert_node(xpointer<_List_node> next_node, const Value& value)
        {
            xpointer<_List_node> new_node = allocate_node(value);
            xpointer<_List_node> prev_node = next_node->_Prev;

            link_nodes(prev_node, new_node);
            link_nodes(new_node, next_node);

            _Mysize = _Mysize + 1;
            return new_node;
        }

        xpointer<_List_node> insert_node(xpointer<_List_node> next_node, Value&& value)
        {
            xpointer<_List_node> new_node = allocate_node(std::move(value));
            xpointer<_List_node> prev_node = next_node->_Prev;

            link_nodes(prev_node, new_node);
            link_nodes(new_node, next_node);

            _Mysize = _Mysize + 1;
            return new_node;
        }

        void erase_node(xpointer<_List_node> node)
        {
            if (node == _Myhead || !node) return;

            link_nodes(node->_Prev, node->_Next);
            deallocate_node(node);
            _Mysize = _Mysize - 1;
        }
    };
}
