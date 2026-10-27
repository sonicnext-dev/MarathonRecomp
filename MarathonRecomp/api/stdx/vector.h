#pragma once

#include <algorithm>
#include <utility>

#include <kernel/heap.h>
#include <kernel/function.h>

namespace stdx
{
    template <typename T>
    class vector
    {
    private:
        be<uint32_t> _Myproxy;
        xpointer<T> _Myfirst;
        xpointer<T> _Mylast;
        xpointer<T> _Myend;

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
                new (target) T(std::move(_Myfirst[i]));
                _Myfirst[i].~T();
            }

            if (_Myfirst)
            {
                _DestroyRange(_Myfirst.get(), _Mylast.get());
                g_userHeap.Free(_Myfirst.get());
            }

            _Myfirst = xpointer<T>(new_block);
            _Mylast = xpointer_add(_Myfirst, old_size);
            _Myend = xpointer_add(_Myfirst, new_capacity);
        }

        void _GrowIfNeeded()
        {
            if (_Mylast.get() == _Myend.get())
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

        vector() noexcept : _Myfirst(nullptr), _Mylast(nullptr), _Myend(nullptr) {}

        explicit vector(size_t count)
        {
            _Myfirst = xpointer<T>(static_cast<T*>(g_userHeap.Alloc(sizeof(T) * count)));
            _Mylast = xpointer_add(_Myfirst, count);
            _Myend = _Mylast;
            _ConstructRange(_Myfirst.get(), _Mylast.get(), T());
        }

        vector(size_t count, const T& value)
        {
            _Myfirst = xpointer<T>(static_cast<T*>(g_userHeap.Alloc(sizeof(T) * count)));
            _Mylast = xpointer_add(_Myfirst, count);
            _Myend = _Mylast;
            _ConstructRange(_Myfirst.get(), _Mylast.get(), value);
        }

        vector(const vector& other)
        {
            _Myfirst = xpointer<T>(static_cast<T*>(g_userHeap.Alloc(sizeof(T) * other.size())));
            _Mylast = xpointer_add(_Myfirst, other.size());
            _Myend = _Mylast;

            for (size_t i = 0; i < other.size(); ++i)
                new (xpointer_add(_Myfirst, i).get()) T(other._Myfirst[i]);
        }

        vector(vector&& other) noexcept : _Myfirst(other._Myfirst), _Mylast(other._Mylast), _Myend(other._Myend)
        {
            other._Myfirst = other._Mylast = other._Myend = nullptr;
        }

        ~vector()
        {
            clear();

            if (_Myfirst)
                g_userHeap.Free(_Myfirst.get());
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

                if (_Myfirst)
                    g_userHeap.Free(_Myfirst.get());

                _Myfirst = other._Myfirst;
                _Mylast = other._Mylast;
                _Myend = other._Myend;

                other._Myfirst = other._Mylast = other._Myend = nullptr;
            }

            return *this;
        }

        T& operator[](size_t pos)
        {
            return _Myfirst.get()[pos];
        }

        const T& operator[](size_t pos) const
        {
            return _Myfirst.get()[pos];
        }

        T& at(size_t pos)
        {
            return _Myfirst.get()[pos];
        }

        const T& at(size_t pos) const
        {
            return _Myfirst.get()[pos];
        }

        T& front()
        {
            return *_Myfirst;
        }

        const T& front() const
        {
            return *_Myfirst;
        }

        T& back()
        {
            return *(xpointer_add(_Mylast, -1).get());
        }

        const T& back() const
        {
            return *(xpointer_add(_Mylast, -1).get());
        }

        T* data()
        {
            return _Myfirst.get();
        }

        const T* data() const
        {
            return _Myfirst.get();
        }

        iterator begin()
        {
            return iterator(_Myfirst);
        }

        const_iterator begin() const
        {
            return const_iterator(_Myfirst);
        }

        const_iterator cbegin() const
        {
            return const_iterator(_Myfirst);
        }

        iterator end()
        {
            return iterator(_Mylast);
        }

        const_iterator end() const
        {
            return const_iterator(_Mylast);
        }

        const_iterator cend() const
        {
            return const_iterator(_Mylast);
        }

        bool empty() const
        {
            return _Myfirst.get() == _Mylast.get();
        }

        size_t size() const
        {
            return (_Mylast.get() - _Myfirst.get());
        }

        size_t capacity() const
        {
            return (_Myend.get() - _Myfirst.get());
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
            _DestroyRange(_Myfirst.get(), _Mylast.get());
            _Mylast = _Myfirst;
        }

        void push_back(const T& value)
        {
            _GrowIfNeeded();
            new (_Mylast.get()) T(value);
            _Mylast = xpointer_add(_Mylast, 1);
        }

        void push_back(T&& value)
        {
            _GrowIfNeeded();
            new (_Mylast.get()) T(std::move(value));
            _Mylast = xpointer_add(_Mylast, 1);
        }

        template <typename... Args>
        void emplace_back(Args&&... args)
        {
            _GrowIfNeeded();
            new (_Mylast.get()) T(std::forward<Args>(args)...);
            _Mylast = xpointer_add(_Mylast, 1);
        }

        void pop_back()
        {
            _Mylast = xpointer_add(_Mylast, -1);
            _Mylast.get()->~T();
        }

        void resize(size_t count)
        {
            if (count < size())
            {
                _DestroyRange(xpointer_add(_Myfirst, count).get(), _Mylast.get());
                _Mylast = xpointer_add(_Myfirst, count);
            }
            else if (count > size())
            {
                reserve(count);

                while (_Mylast.get() != xpointer_add(_Myfirst, count).get())
                {
                    new (_Mylast.get()) T();
                    _Mylast = xpointer_add(_Mylast, 1);
                }
            }
        }

        void resize(size_t count, const T& value)
        {
            if (count < size())
            {
                _DestroyRange(xpointer_add(_Myfirst, count).get(), _Mylast.get());
                _Mylast = xpointer_add(_Myfirst, count);
            }
            else if (count > size())
            {
                reserve(count);

                while (_Mylast.get() != xpointer_add(_Myfirst, count).get())
                {
                    new (_Mylast.get()) T(value);
                    _Mylast = xpointer_add(_Mylast, 1);
                }
            }
        }

        void swap(vector& other) noexcept
        {
            std::swap(_Myfirst, other._Myfirst);
            std::swap(_Mylast, other._Mylast);
            std::swap(_Myend, other._Myend);
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
