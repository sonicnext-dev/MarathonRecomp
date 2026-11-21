#pragma once

#include <Marathon.inl>
#include <Common/ArrayIterator.h>
#include <Kaim/CObject.h>

namespace Kaim
{
    template <typename T>
    class CArray : public CObject
    {
    public:
        xpointer<T> m_pool;
        be<uint32_t> m_capacity;
        be<uint32_t> m_count;

        T* at(uint32_t in_index) const
        {
            return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(m_pool.get()) + (sizeof(T) * in_index));
        }

        T* operator[](const uint32_t in_index)
        {
            return at(in_index);
        }

        const T* operator[](const uint32_t in_index) const
        {
            return at(in_index);
        }

        ArrayIterator<T> begin()
        {
            return ArrayIterator<T>(at(0));
        }

        ArrayIterator<T> end()
        {
            return ArrayIterator<T>(at(m_count));
        }
    };

    MARATHON_ASSERT_OFFSETOF(CArray<void>, m_pool, 0x04);
    MARATHON_ASSERT_OFFSETOF(CArray<void>, m_capacity, 0x08);
    MARATHON_ASSERT_OFFSETOF(CArray<void>, m_count, 0x0C);
    MARATHON_ASSERT_SIZEOF(CArray<void>, 0x10);
}
