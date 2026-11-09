#pragma once

#include <Marathon.inl>
#include <Common/ArrayIterator.h>

namespace hk330
{
    template <typename T>
    class hkArray
    {
    public:
        xpointer<T> m_data;
        be<uint32_t> m_size;
        be<uint32_t> m_capacityAndFlags;

        T* at(uint32_t in_index) const
        {
            return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(m_data.get()) + (sizeof(T) * in_index));
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
            return ArrayIterator<T>(at(m_size));
        }
    };

    MARATHON_ASSERT_OFFSETOF(hkArray<void>, m_data, 0x00);
    MARATHON_ASSERT_OFFSETOF(hkArray<void>, m_size, 0x04);
    MARATHON_ASSERT_OFFSETOF(hkArray<void>, m_capacityAndFlags, 0x08);
    MARATHON_ASSERT_SIZEOF(hkArray<void>, 0x0C);
}
