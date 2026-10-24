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
        T& operator[](uint32_t in_index)
        {
            return m_Items[in_index];
        }

        const T& operator[](uint32_t in_index) const
        {
            return m_Items[in_index];
        }

        uint32_t size() const 
        {
            return Length;
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
