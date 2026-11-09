#pragma once

#include <Marathon.h>
#include <Common/ArrayIterator.h>

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

        ArrayIterator<T> begin()
        {
            return ArrayIterator<T>(&m_Items[0]);
        }

        ArrayIterator<T> end()
        {
            return ArrayIterator<T>(&m_Items[Length]);
        }
    };
}
