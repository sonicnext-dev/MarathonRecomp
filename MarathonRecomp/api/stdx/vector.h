#pragma once

#include <kernel/heap.h>
#include <kernel/function.h>
#include <utility>


namespace stdx
{
    template<typename T>
    class vector
    {
    private:
        be<uint32_t> _Myproxy;
        xpointer<T> _MyFirst;
        xpointer<T> _MyLast;
        xpointer<T> _MyEnd;

        void _ConstructRange(T* first, T* last, const T& value)
        {
            for (; first != last; ++first)
            {
                new (first) T(value);
            }
        }

        void _DestroyRange(T* first, T* last)
        {
            while (first != last)
            {
                first->~T();
                ++first;
            }
        }

        void _Reallocate(size_t newCapacity)
        {
            T* newBlock = static_cast<T*>(g_userHeap.Alloc(sizeof(T) * newCapacity));
            const size_t oldSize = size();
            try
            {
                for (size_t i = 0; i < oldSize; ++i)
                {
                    new (newBlock + i) T(std::move(_MyFirst[i]));
                    _MyFirst[i].~T();
                }
            }
            catch (...)
            {
                _DestroyRange(newBlock, newBlock + oldSize);
                g_userHeap.Free(newBlock);
                throw;
            }

            if (_MyFirst)
            {
                _DestroyRange(_MyFirst, _MyLast);
                g_userHeap.Free(_MyFirst.get());
            }

            _MyFirst = xpointer<T>(newBlock);
            _MyLast = xpointer<T>(newBlock + oldSize);
            _MyEnd = xpointer<T>(newBlock + newCapacity);
        }

        void _GrowIfNeeded()
        {
            if (_MyLast.get() == _MyEnd.get())
            {
                _Reallocate(size() ? size() * 2 : 1);
            }
        }

    public:
        // Iterator
        using iterator = xpointer<T>;
        using const_iterator = xpointer<const T>;


        vector() noexcept
            : _MyFirst(nullptr)
            , _MyLast(nullptr)
            , _MyEnd(nullptr)
        {
        }

        explicit vector(size_t count)
            : _MyFirst(static_cast<T*>(g_userHeap.Alloc(sizeof(T)* count)))
            , _MyLast(_MyFirst.get() + count)
            , _MyEnd(_MyLast)
        {
            _ConstructRange(_MyFirst, _MyLast, T());
        }

        vector(size_t count, const T& value)
            : _MyFirst(static_cast<T*>(g_userHeap.Alloc(sizeof(T)* count)))
            , _MyLast(_MyFirst.get() + count)
            , _MyEnd(_MyLast)
        {
            _ConstructRange(_MyFirst, _MyLast, value);
        }

        vector(const vector& other)
            : _MyFirst(static_cast<T*>(g_userHeap.Alloc(sizeof(T) * other.size())))
            , _MyLast(_MyFirst.get() + other.size())
            , _MyEnd(_MyLast)
        {
            try
            {
                for (size_t i = 0; i < other.size(); ++i)
                {
                    new (_MyFirst.get() + i) T(other._MyFirst[i]);
                }
            }
            catch (...)
            {
                _DestroyRange(_MyFirst, _MyLast);
                g_userHeap.Free(_MyFirst.get());
                throw;
            }
        }

        vector(vector&& other) noexcept
            : _MyFirst(other._MyFirst)
            , _MyLast(other._MyLast)
            , _MyEnd(other._MyEnd)
        {
            other._MyFirst = other._MyLast = other._MyEnd = nullptr;
        }

        ~vector()
        {
            _DestroyRange(_MyFirst, _MyLast);
            if (_MyFirst)
            {
                g_userHeap.Free(_MyFirst.get());
            }
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
                _DestroyRange(_MyFirst, _MyLast);
                if (_MyFirst)
                {
                    g_userHeap.Free(_MyFirst.get());
                }

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
            if (pos >= size()) throw std::out_of_range("vector index out of range");
            return _MyFirst.get()[pos];
        }

        const T& at(size_t pos) const
        {
            if (pos >= size()) throw std::out_of_range("vector index out of range");
            return _MyFirst.get()[pos];
        }

        T& front() {
            return *_MyFirst;
        }
        const T& front() const {
            return *_MyFirst;
        }
        T& back() {
            return *(_MyLast.get() - 1); 
        }
        const T& back() const 
        {
            return *(_MyLast.get() - 1); 
        }
        T* data() 
        {
            return _MyFirst;
        }
        const T* data() const {
            return _MyFirst;
        }

        // Iterators
        iterator begin() {
            return _MyFirst;
        }
        const_iterator begin() const 
        { 
            return _MyFirst;
        }
        const_iterator cbegin() const
        { 
            return _MyFirst;
        }
        iterator end() 
        {
            return _MyLast;
        }
        const_iterator end() const 
        { 
            return _MyLast;
        }
        const_iterator cend() const 
        {
            return _MyLast;
        }

        // Capacity
        bool empty() const 
        { 
            return _MyFirst.get() == _MyLast.get();
        }
        size_t size() const
        { 
            return _MyLast.get() - _MyFirst.get();
        }

        size_t capacity() const
        {
            return _MyEnd.get() - _MyFirst.get();
        }

        void reserve(size_t newCapacity)
        {
            if (newCapacity > capacity())
            {
                _Reallocate(newCapacity);
            }
        }

        void shrink_to_fit()
        {
            if (size() < capacity())
            {
                _Reallocate(size());
            }
        }

        void clear()
        {
            _DestroyRange(_MyFirst, _MyLast);
            _MyLast = _MyFirst;
        }

        void push_back(const T& value)
        {
            _GrowIfNeeded();
            new (_MyLast.get()) T(value);
            _MyLast = xpointer<T>(_MyLast.get() + 1);
        }

        void push_back(T&& value)
        {
            _GrowIfNeeded();
            new (_MyLast.get()) T(std::move(value));
            _MyLast = xpointer<T>(_MyLast.get() + 1);
        }

        template<typename... Args>
        void emplace_back(Args&&... args)
        {
            _GrowIfNeeded();
            new (_MyLast.get()) T(std::forward<Args>(args)...);
            _MyLast = xpointer<T>(_MyLast.get() + 1);
        }

        void pop_back()
        {
            (_MyLast.get() - 1)->~T();
            _MyLast = xpointer<T>(_MyLast.get() - 1);
        }

        void resize(size_t count)
        {
            if (count < size())
            {
                _DestroyRange(_MyFirst.get() + count, _MyLast);
                _MyLast = xpointer<T>(_MyFirst.get() + count);
            }
            else if (count > size())
            {
                reserve(count);
                while (_MyLast.get() != _MyFirst.get() + count)
                {
                    new (_MyLast.get()) T();
                    _MyLast = xpointer<T>(_MyLast.get() + 1);
                }
            }
        }

        void resize(size_t count, const T& value)
        {
            if (count < size())
            {
                _DestroyRange(_MyFirst.get() + count, _MyLast);
                _MyLast = xpointer<T>(_MyFirst.get() + count);
            }
            else if (count > size())
            {
                reserve(count);
                while (_MyLast.get() != _MyFirst.get() + count)
                {
                    new (_MyLast.get()) T(value);
                    _MyLast = xpointer<T>(_MyLast.get() + 1);
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

    template<typename T>
    void swap(vector<T>& lhs, vector<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template<typename T>
    bool operator==(const vector<T>& lhs, const vector<T>& rhs)
    {
        return lhs.size() == rhs.size() &&
            std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T>
    bool operator!=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename T>
    bool operator<(const vector<T>& lhs, const vector<T>& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end());
    }

    template<typename T>
    bool operator<=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template<typename T>
    bool operator>(const vector<T>& lhs, const vector<T>& rhs)
    {
        return rhs < lhs;
    }

    template<typename T>
    bool operator>=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs < rhs);
    }


}
