#pragma once

#include <kernel/heap.h>
#include <kernel/function.h>
#include <utility>
#include <algorithm>

namespace stdx
{
    template <typename T>
    class vector
    {
    private:
        be<uint32_t> _Myproxy;
        xpointer<T> _MyFirst;
        xpointer<T> _MyLast;
        xpointer<T> _MyEnd;

        static xpointer<T> xpointer_add(xpointer<T> ptr, size_t offset)
        {
            return xpointer<T>(reinterpret_cast<T*>(
                reinterpret_cast<uintptr_t>(ptr.get()) + offset * sizeof(T)));
        }

        void _ConstructRange(T* first, T* last, const T& value)
        {
            for (; first != last; ++first)
                new (first) T(value);
        }

        void _DestroyRange(T* first, T* last)
        {
            while (first != last)
            {
                first->~T();
                ++first;
            }
        }

        void _Reallocate(size_t new_capacity)
        {
            T* new_block = static_cast<T*>(g_userHeap.Alloc(sizeof(T) * new_capacity));
            const size_t old_size = size();

            for (size_t i = 0; i < old_size; ++i)
            {
                T* target = reinterpret_cast<T*>(
                    reinterpret_cast<uintptr_t>(new_block) + i * sizeof(T));
                new (target) T(std::move(_MyFirst[i]));
                _MyFirst[i].~T();
            }

            if (_MyFirst)
            {
                _DestroyRange(_MyFirst.get(), _MyLast.get());
                g_userHeap.Free(_MyFirst.get());
            }

            _MyFirst = xpointer<T>(new_block);
            _MyLast = xpointer_add(_MyFirst, old_size);
            _MyEnd = xpointer_add(_MyFirst, new_capacity);
        }

        void _GrowIfNeeded()
        {
            if (_MyLast.get() == _MyEnd.get())
                _Reallocate(size() ? size() * 2 : 1);
        }

        class iterator_wrapper
        {
        private:
            xpointer<T> _ptr;

        public:
            explicit iterator_wrapper(xpointer<T> ptr) : _ptr(ptr) {}

            T& operator*() const
            {
                return *_ptr;
            }

            T* operator->() const
            {
                return _ptr.get();
            }

            iterator_wrapper& operator++()
            {
                _ptr = xpointer<T>(reinterpret_cast<T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) + sizeof(T)));

                return *this;
            }

            iterator_wrapper operator++(int)
            {
                iterator_wrapper tmp = *this;
                ++(*this);
                return tmp;
            }

            iterator_wrapper& operator--()
            {
                _ptr = xpointer<T>(reinterpret_cast<T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) - sizeof(T)));

                return *this;
            }

            iterator_wrapper operator--(int)
            {
                iterator_wrapper tmp = *this;
                --(*this);
                return tmp;
            }

            iterator_wrapper operator+(size_t n) const
            {
                return iterator_wrapper(xpointer<T>(reinterpret_cast<T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) + n * sizeof(T))));
            }

            iterator_wrapper operator-(size_t n) const
            {
                return iterator_wrapper(xpointer<T>(reinterpret_cast<T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) - n * sizeof(T))));
            }

            bool operator==(const iterator_wrapper& other) const
            {
                return _ptr.get() == other._ptr.get();
            }

            bool operator!=(const iterator_wrapper& other) const
            {
                return _ptr.get() != other._ptr.get();
            }
        };

        class const_iterator_wrapper
        {
        private:
            xpointer<const T> _ptr;

        public:
            explicit const_iterator_wrapper(xpointer<const T> ptr) : _ptr(ptr) {}

            const T& operator*() const
            {
                return *_ptr;
            }

            const T* operator->() const
            {
                return _ptr.get();
            }

            const_iterator_wrapper& operator++()
            {
                _ptr = xpointer<const T>(reinterpret_cast<const T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) + sizeof(T)));

                return *this;
            }

            const_iterator_wrapper operator++(int)
            {
                const_iterator_wrapper tmp = *this;
                ++(*this);
                return tmp;
            }

            const_iterator_wrapper& operator--()
            {
                _ptr = xpointer<const T>(reinterpret_cast<const T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) - sizeof(T)));

                return *this;
            }

            const_iterator_wrapper operator--(int)
            {
                const_iterator_wrapper tmp = *this;
                --(*this);
                return tmp;
            }

            const_iterator_wrapper operator+(size_t n) const
            {
                return const_iterator_wrapper(xpointer<const T>(reinterpret_cast<const T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) + n * sizeof(T))));
            }

            const_iterator_wrapper operator-(size_t n) const
            {
                return const_iterator_wrapper(xpointer<const T>(reinterpret_cast<const T*>(
                    reinterpret_cast<uintptr_t>(_ptr.get()) - n * sizeof(T))));
            }

            bool operator==(const const_iterator_wrapper& other) const
            {
                return _ptr.get() == other._ptr.get();
            }

            bool operator!=(const const_iterator_wrapper& other) const
            {
                return _ptr.get() != other._ptr.get();
            }
        };

    public:
        using iterator = iterator_wrapper;
        using const_iterator = const_iterator_wrapper;

        vector() noexcept : _MyFirst(nullptr), _MyLast(nullptr), _MyEnd(nullptr) {}

        explicit vector(size_t count)
        {
            _MyFirst = xpointer<T>(static_cast<T*>(g_userHeap.Alloc(sizeof(T) * count)));
            _MyLast = xpointer_add(_MyFirst, count);
            _MyEnd = _MyLast;
            _ConstructRange(_MyFirst.get(), _MyLast.get(), T());
        }

        vector(size_t count, const T& value)
        {
            _MyFirst = xpointer<T>(static_cast<T*>(g_userHeap.Alloc(sizeof(T) * count)));
            _MyLast = xpointer_add(_MyFirst, count);
            _MyEnd = _MyLast;
            _ConstructRange(_MyFirst.get(), _MyLast.get(), value);
        }

        vector(const vector& other)
        {
            _MyFirst = xpointer<T>(static_cast<T*>(g_userHeap.Alloc(sizeof(T) * other.size())));
            _MyLast = xpointer_add(_MyFirst, other.size());
            _MyEnd = _MyLast;

            for (size_t i = 0; i < other.size(); ++i)
                new (xpointer_add(_MyFirst, i).get()) T(other._MyFirst[i]);
        }

        vector(vector&& other) noexcept : _MyFirst(other._MyFirst), _MyLast(other._MyLast), _MyEnd(other._MyEnd)
        {
            other._MyFirst = other._MyLast = other._MyEnd = nullptr;
        }

        ~vector()
        {
            clear();

            if (_MyFirst)
                g_userHeap.Free(_MyFirst.get());
        }

        vector& operator=(const vector& other)
        {
            if (this != &other)
            {
                vector tmp(other);
                swap(tmp);
            }

            return *this;
        }

        vector& operator=(vector&& other) noexcept
        {
            if (this != &other)
            {
                clear();

                if (_MyFirst)
                    g_userHeap.Free(_MyFirst.get());

                _MyFirst = other._MyFirst;
                _MyLast = other._MyLast;
                _MyEnd = other._MyEnd;

                other._MyFirst = other._MyLast = other._MyEnd = nullptr;
            }

            return *this;
        }

        T& operator[](size_t pos)
        {
            return _MyFirst.get()[pos];
        }

        const T& operator[](size_t pos) const
        {
            return _MyFirst.get()[pos];
        }

        T& at(size_t pos)
        {
            return _MyFirst.get()[pos];
        }

        const T& at(size_t pos) const
        {
            return _MyFirst.get()[pos];
        }

        T& front()
        {
            return *_MyFirst;
        }

        const T& front() const
        {
            return *_MyFirst;
        }

        T& back()
        {
            return *(xpointer_add(_MyLast, -1).get());
        }

        const T& back() const
        {
            return *(xpointer_add(_MyLast, -1).get());
        }

        T* data()
        {
            return _MyFirst.get();
        }

        const T* data() const
        {
            return _MyFirst.get();
        }

        iterator begin()
        {
            return iterator(_MyFirst);
        }

        const_iterator begin() const
        {
            return const_iterator(_MyFirst);
        }

        const_iterator cbegin() const
        {
            return const_iterator(_MyFirst);
        }

        iterator end()
        {
            return iterator(_MyLast);
        }

        const_iterator end() const
        {
            return const_iterator(_MyLast);
        }

        const_iterator cend() const
        {
            return const_iterator(_MyLast);
        }

        bool empty() const
        {
            return _MyFirst.get() == _MyLast.get();
        }

        size_t size() const
        {
            return (_MyLast.get() - _MyFirst.get());
        }

        size_t capacity() const
        {
            return (_MyEnd.get() - _MyFirst.get());
        }

        void reserve(size_t new_capacity)
        {
            if (new_capacity > capacity())
                _Reallocate(new_capacity);
        }

        void shrink_to_fit()
        {
            if (size() < capacity())
                _Reallocate(size());
        }

        void clear()
        {
            _DestroyRange(_MyFirst.get(), _MyLast.get());
            _MyLast = _MyFirst;
        }

        void push_back(const T& value)
        {
            _GrowIfNeeded();
            new (_MyLast.get()) T(value);
            _MyLast = xpointer_add(_MyLast, 1);
        }

        void push_back(T&& value)
        {
            _GrowIfNeeded();
            new (_MyLast.get()) T(std::move(value));
            _MyLast = xpointer_add(_MyLast, 1);
        }

        template <typename... Args>
        void emplace_back(Args&&... args)
        {
            _GrowIfNeeded();
            new (_MyLast.get()) T(std::forward<Args>(args)...);
            _MyLast = xpointer_add(_MyLast, 1);
        }

        void pop_back()
        {
            _MyLast = xpointer_add(_MyLast, -1);
            _MyLast.get()->~T();
        }

        void resize(size_t count)
        {
            if (count < size())
            {
                _DestroyRange(xpointer_add(_MyFirst, count).get(), _MyLast.get());
                _MyLast = xpointer_add(_MyFirst, count);
            }
            else if (count > size())
            {
                reserve(count);

                while (_MyLast.get() != xpointer_add(_MyFirst, count).get())
                {
                    new (_MyLast.get()) T();
                    _MyLast = xpointer_add(_MyLast, 1);
                }
            }
        }

        void resize(size_t count, const T& value)
        {
            if (count < size())
            {
                _DestroyRange(xpointer_add(_MyFirst, count).get(), _MyLast.get());
                _MyLast = xpointer_add(_MyFirst, count);
            }
            else if (count > size())
            {
                reserve(count);

                while (_MyLast.get() != xpointer_add(_MyFirst, count).get())
                {
                    new (_MyLast.get()) T(value);
                    _MyLast = xpointer_add(_MyLast, 1);
                }
            }
        }

        void swap(vector& other) noexcept
        {
            std::swap(_MyFirst, other._MyFirst);
            std::swap(_MyLast, other._MyLast);
            std::swap(_MyEnd, other._MyEnd);
        }
    };

    template <typename T>
    void swap(vector<T>& lhs, vector<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <typename T>
    bool operator==(const vector<T>& lhs, const vector<T>& rhs)
    {
        return lhs.size() == rhs.size()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T>
    bool operator!=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T>
    bool operator<(const vector<T>& lhs, const vector<T>& rhs)
    {
        return std::lexicographical_compare(
            lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename T>
    bool operator<=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename T>
    bool operator>(const vector<T>& lhs, const vector<T>& rhs)
    {
        return rhs < lhs;
    }

    template <typename T>
    bool operator>=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs < rhs);
    }
}