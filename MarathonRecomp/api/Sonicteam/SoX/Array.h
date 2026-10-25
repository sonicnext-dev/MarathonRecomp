#pragma once

#include <Marathon.h>

namespace Sonicteam::SoX
{
    template <typename T, uint32_t Length>
    class Array
    {
    private:
        T m_Items[Length];

    public:
        T& operator[](const uint32_t in_index)
        {
            return m_Items[in_index];
        }

        const T& operator[](const uint32_t in_index) const
        {
            return m_Items[in_index];
        }

        uint32_t Capacity() const 
        {
            return Length;
        }

        void Clear()
        {
            for (int i = 0; i < Length; i++)
                m_Items[i] = T();
        }

        bool Insert(const uint32_t in_index, const T& in_rItem)
        {
            if (in_index >= Length)
                return false;

            // Shift existing items forwards.
            for (uint32_t i = Length - 1; i > in_index; i--)
                m_Items[i] = std::move(m_Items[i - 1]);

            m_Items[in_index] = in_rItem;

            return true;
        }

        bool RemoveAt(const uint32_t in_index)
        {
            if (in_index >= Length)
                return false;

            // Shift existing items backwards.
            for (uint32_t i = in_index; i < Length - 1; i++)
                m_Items[i] = std::move(m_Items[i + 1]);

            m_Items[Length - 1] = T();

            return true;
        }

        class iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

        private:
            pointer m_pCurrent{};

        public:
            explicit iterator(pointer in_pStart) : m_pCurrent(in_pStart) {}

            reference operator*() const
            {
                return *m_pCurrent;
            }

            pointer operator->()
            {
                return m_pCurrent;
            }

            iterator& operator++()
            {
                m_pCurrent++;
                return *this;
            }

            iterator operator++(int)
            {
                const auto _ = *this;
                m_pCurrent++;
                return _;
            }

            friend bool operator==(const iterator& in_rLeft, const iterator& in_rRight)
            {
                return in_rLeft.m_pCurrent == in_rRight.m_pCurrent;
            }

            friend bool operator!=(const iterator& in_rLeft, const iterator& in_rRight)
            {
                return in_rLeft.m_pCurrent != in_rRight.m_pCurrent;
            }
        };

        iterator begin()
        {
            return iterator(&m_Items[0]);
        }

        iterator end()
        {
            return iterator(&m_Items[Length]);
        }
    };
}
