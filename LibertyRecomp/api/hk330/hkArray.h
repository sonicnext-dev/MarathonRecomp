#pragma once

#include <Liberty.inl>

namespace hk330
{
    template <typename T>
    class hkArray
    {
    public:
        xpointer<T> m_data;
        be<uint32_t> m_size;
        be<uint32_t> m_capacityAndFlags;

        template <typename E>
        T* GetIndex(E i)
        {
            return (T*)(m_data.get() + ((int)i * sizeof(T)));
        }
    };

    LIBERTY_ASSERT_OFFSETOF(hkArray<void>, m_data, 0x00);
    LIBERTY_ASSERT_OFFSETOF(hkArray<void>, m_size, 0x04);
    LIBERTY_ASSERT_OFFSETOF(hkArray<void>, m_capacityAndFlags, 0x08);
}
