#pragma once

#include <Marathon.inl>

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
}
