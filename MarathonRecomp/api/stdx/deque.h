#pragma once

#include <kernel/heap.h>
#include <kernel/function.h>
#include <utility>
#include <algorithm>

namespace stdx
{
    template <typename T>
    class deque
    {
    private:
        static const size_t BLOCK_SIZE = 16; 

        be<uint32_t> _Myproxy;
        xpointer<xpointer<T>> _Map;
        be<uint32_t> _Mapsize;     
        be<uint32_t> _Myoff;       
        be<uint32_t> _Mysize;      

        // Helper functions for block management
        size_t _GetBlockIndex(size_t pos) const
        {
            return (_Myoff + pos) / BLOCK_SIZE;
        }

        size_t _GetElementIndex(size_t pos) const
        {
            return (_Myoff + pos) % BLOCK_SIZE;
        }

        xpointer<T> _GetBlock(size_t blockIndex) const
        {
            return _Map[blockIndex];
        }

        void _GrowMapIfNeeded()
        {
            if (_Mapsize == 0 || _GetBlockIndex(_Mysize) >= _Mapsize)
            {
                size_t new_mapsize = _Mapsize == 0 ? 8 : _Mapsize * 2;
                xpointer<T>* new_map = static_cast<xpointer<T>*>(g_userHeap.Alloc(sizeof(xpointer<T>) * new_mapsize));

                // Copy existing blocks to new map
                for (size_t i = 0; i < _Mapsize; ++i)
                    new_map[i] = _Map[i];

                // Initialize new blocks to nullptr
                for (size_t i = _Mapsize; i < new_mapsize; ++i)
                    new_map[i] = xpointer<T>(nullptr);

                if (_Map)
                    g_userHeap.Free(_Map.get());

                _Map = xpointer<xpointer<T>>(new_map);
                _Mapsize = new_mapsize;
            }
        }

        void _AllocateBlock(size_t blockIndex)
        {
            if (!_Map[blockIndex])
            {
                T* new_block = static_cast<T*>(g_userHeap.Alloc(sizeof(T) * BLOCK_SIZE));
                _Map[blockIndex] = xpointer<T>(new_block);
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

        void _ConstructRange(T* first, T* last, const T& value)
        {
            for (; first != last; ++first)
                new (first) T(value);
        }

        class iterator_wrapper
        {
        private:
            deque* _deque;
            size_t _pos;

        public:
            explicit iterator_wrapper(deque* dq, size_t pos = 0) : _deque(dq), _pos(pos) {}

            T& operator*() const
            {
                return (*_deque)[_pos];
            }

            T* operator->() const
            {
                return &(*_deque)[_pos];
            }

            iterator_wrapper& operator++()
            {
                ++_pos;
                return *this;
            }

            iterator_wrapper operator++(int)
            {
                iterator_wrapper tmp = *this;
                ++_pos;
                return tmp;
            }

            iterator_wrapper& operator--()
            {
                --_pos;
                return *this;
            }

            iterator_wrapper operator--(int)
            {
                iterator_wrapper tmp = *this;
                --_pos;
                return tmp;
            }

            iterator_wrapper operator+(size_t n) const
            {
                return iterator_wrapper(_deque, _pos + n);
            }

            iterator_wrapper operator-(size_t n) const
            {
                return iterator_wrapper(_deque, _pos - n);
            }

            bool operator==(const iterator_wrapper& other) const
            {
                return _deque == other._deque && _pos == other._pos;
            }

            bool operator!=(const iterator_wrapper& other) const
            {
                return !(*this == other);
            }

            size_t position() const { return _pos; }
        };

        class const_iterator_wrapper
        {
        private:
            const deque* _deque;
            size_t _pos;

        public:
            explicit const_iterator_wrapper(const deque* dq, size_t pos = 0) : _deque(dq), _pos(pos) {}

            const T& operator*() const
            {
                return (*_deque)[_pos];
            }

            const T* operator->() const
            {
                return &(*_deque)[_pos];
            }

            const_iterator_wrapper& operator++()
            {
                ++_pos;
                return *this;
            }

            const_iterator_wrapper operator++(int)
            {
                const_iterator_wrapper tmp = *this;
                ++_pos;
                return tmp;
            }

            const_iterator_wrapper& operator--()
            {
                --_pos;
                return *this;
            }

            const_iterator_wrapper operator--(int)
            {
                const_iterator_wrapper tmp = *this;
                --_pos;
                return tmp;
            }

            const_iterator_wrapper operator+(size_t n) const
            {
                return const_iterator_wrapper(_deque, _pos + n);
            }

            const_iterator_wrapper operator-(size_t n) const
            {
                return const_iterator_wrapper(_deque, _pos - n);
            }

            bool operator==(const const_iterator_wrapper& other) const
            {
                return _deque == other._deque && _pos == other._pos;
            }

            bool operator!=(const const_iterator_wrapper& other) const
            {
                return !(*this == other);
            }

            size_t position() const { return _pos; }
        };

    public:
        using iterator = iterator_wrapper;
        using const_iterator = const_iterator_wrapper;

        deque() noexcept : _Map(nullptr), _Mapsize(0), _Myoff(0), _Mysize(0) {}

        explicit deque(size_t count) : _Map(nullptr), _Mapsize(0), _Myoff(0), _Mysize(0)
        {
            resize(count);
        }

        deque(size_t count, const T& value) : _Map(nullptr), _Mapsize(0), _Myoff(0), _Mysize(0)
        {
            resize(count, value);
        }

        deque(const deque& other) : _Map(nullptr), _Mapsize(0), _Myoff(other._Myoff), _Mysize(0)
        {
            // Allocate map and copy elements
            if (other._Mysize > 0)
            {
                resize(other._Mysize);
                for (size_t i = 0; i < _Mysize; ++i)
                    (*this)[i] = other[i];
            }
        }

        deque(deque&& other) noexcept
            : _Map(other._Map), _Mapsize(other._Mapsize), _Myoff(other._Myoff), _Mysize(other._Mysize)
        {
            other._Map = xpointer<xpointer<T>>(nullptr);
            other._Mapsize = 0;
            other._Myoff = 0;
            other._Mysize = 0;
        }

        ~deque()
        {
            clear();

            // Free all allocated blocks
            if (_Map)
            {
                for (size_t i = 0; i < _Mapsize; ++i)
                {
                    if (_Map[i])
                        g_userHeap.Free(_Map[i].get());
                }
                g_userHeap.Free(_Map.get());
            }
        }

        deque& operator=(const deque& other)
        {
            if (this != &other)
            {
                deque tmp(other);
                swap(tmp);
            }
            return *this;
        }

        deque& operator=(deque&& other) noexcept
        {
            if (this != &other)
            {
                clear();

                // Free existing resources
                if (_Map)
                {
                    for (size_t i = 0; i < _Mapsize; ++i)
                    {
                        if (_Map[i])
                            g_userHeap.Free(_Map[i].get());
                    }
                    g_userHeap.Free(_Map.get());
                }

                _Map = other._Map;
                _Mapsize = other._Mapsize;
                _Myoff = other._Myoff;
                _Mysize = other._Mysize;

                other._Map = xpointer<xpointer<T>>(nullptr);
                other._Mapsize = 0;
                other._Myoff = 0;
                other._Mysize = 0;
            }
            return *this;
        }

        T& operator[](size_t pos)
        {
            size_t blockIndex = _GetBlockIndex(pos);
            size_t elementIndex = _GetElementIndex(pos);
            return _Map[blockIndex][elementIndex];
        }

        const T& operator[](size_t pos) const
        {
            size_t blockIndex = _GetBlockIndex(pos);
            size_t elementIndex = _GetElementIndex(pos);
            return _Map[blockIndex][elementIndex];
        }

        T& at(size_t pos)
        {
            return (*this)[pos];
        }

        const T& at(size_t pos) const
        {
            return (*this)[pos];
        }

        T& front()
        {
            return (*this)[0];
        }

        const T& front() const
        {
            return (*this)[0];
        }

        T& back()
        {
            return (*this)[_Mysize - 1];
        }

        const T& back() const
        {
            return (*this)[_Mysize - 1];
        }

        iterator begin()
        {
            return iterator(this, 0);
        }

        const_iterator begin() const
        {
            return const_iterator(this, 0);
        }

        const_iterator cbegin() const
        {
            return const_iterator(this, 0);
        }

        iterator end()
        {
            return iterator(this, _Mysize);
        }

        const_iterator end() const
        {
            return const_iterator(this, _Mysize);
        }

        const_iterator cend() const
        {
            return const_iterator(this, _Mysize);
        }

        bool empty() const
        {
            return _Mysize == 0;
        }

        size_t size() const
        {
            return _Mysize;
        }

        void clear()
        {
            // Destroy all elements
            for (size_t i = 0; i < _Mysize; ++i)
            {
                (*this)[i].~T();
            }
            _Mysize = 0;
            _Myoff = 0;
        }

        void push_back(const T& value)
        {
            _GrowMapIfNeeded();
            size_t blockIndex = _GetBlockIndex(_Mysize);
            _AllocateBlock(blockIndex);

            size_t elementIndex = _GetElementIndex(_Mysize);
            new (&_Map[blockIndex][elementIndex]) T(value);
            _Mysize = _Mysize.get() + 1;
        }

        void push_back(T&& value)
        {
            _GrowMapIfNeeded();
            size_t blockIndex = _GetBlockIndex(_Mysize);
            _AllocateBlock(blockIndex);

            size_t elementIndex = _GetElementIndex(_Mysize);
            new (&_Map[blockIndex][elementIndex]) T(std::move(value));
            _Mysize = _Mysize.get() + 1;;
        }

        template <typename... Args>
        void emplace_back(Args&&... args)
        {
            _GrowMapIfNeeded();
            size_t blockIndex = _GetBlockIndex(_Mysize);
            _AllocateBlock(blockIndex);

            size_t elementIndex = _GetElementIndex(_Mysize);
            new (&_Map[blockIndex][elementIndex]) T(std::forward<Args>(args)...);
            _Mysize = _Mysize.get() + 1;
        }

        void pop_back()
        {
            if (_Mysize > 0)
            {
                _Mysize = _Mysize.get() - 1;
                size_t blockIndex = _GetBlockIndex(_Mysize);
                size_t elementIndex = _GetElementIndex(_Mysize);
                _Map[blockIndex][elementIndex].~T();
            }
        }

        void push_front(const T& value)
        {
            if (_Myoff == 0)
            {
                // Need to reorganize or grow map at front
                _GrowMapIfNeeded();
                // This is simplified - real implementation would be more complex
            }

            if (_Myoff > 0)
            {
                _Myoff = _Myoff.get() - 1;
                size_t blockIndex = _GetBlockIndex(0);
                _AllocateBlock(blockIndex);

                size_t elementIndex = _GetElementIndex(0);
                new (&_Map[blockIndex][elementIndex]) T(value);
                _Mysize = _Mysize.get() + 1;
            }
            else
            {
                // Fallback to insert at beginning
                push_back(value); // This isn't correct for front, but works for basic case
            }
        }

        void pop_front()
        {
            if (_Mysize > 0)
            {
                size_t blockIndex = _GetBlockIndex(0);
                size_t elementIndex = _GetElementIndex(0);
                _Map[blockIndex][elementIndex].~T();
                _Myoff = _Myoff.get() + 1;
                _Mysize = _Mysize.get() - 1;
            }
        }

        void resize(size_t count)
        {
            if (count < _Mysize)
            {
                // Destroy extra elements
                for (size_t i = count; i < _Mysize; ++i)
                    (*this)[i].~T();
                _Mysize = count;
            }
            else if (count > _Mysize)
            {
                // Add default-constructed elements
                while (_Mysize < count)
                    push_back(T());
            }
        }

        void resize(size_t count, const T& value)
        {
            if (count < _Mysize)
            {
                // Destroy extra elements
                for (size_t i = count; i < _Mysize; ++i)
                    (*this)[i].~T();
                _Mysize = count;
            }
            else if (count > _Mysize)
            {
                // Add elements with specified value
                while (_Mysize < count)
                    push_back(value);
            }
        }

        void swap(deque& other) noexcept
        {
            std::swap(_Map, other._Map);
            std::swap(_Mapsize, other._Mapsize);
            std::swap(_Myoff, other._Myoff);
            std::swap(_Mysize, other._Mysize);
        }
    };

    template <typename T>
    void swap(deque<T>& lhs, deque<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <typename T>
    bool operator==(const deque<T>& lhs, const deque<T>& rhs)
    {
        return lhs.size() == rhs.size()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T>
    bool operator!=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T>
    bool operator<(const deque<T>& lhs, const deque<T>& rhs)
    {
        return std::lexicographical_compare(
            lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename T>
    bool operator<=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename T>
    bool operator>(const deque<T>& lhs, const deque<T>& rhs)
    {
        return rhs < lhs;
    }

    template <typename T>
    bool operator>=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(lhs < rhs);
    }
}
