#pragma once

#include <Marathon.inl>

namespace hk330
{
    template <typename T>
    class hkArray
    {
    public:
        xpointer<T> m_ptr;
        uint32_t m_numElements;
        uint32_t m_maxElements;

        template <typename P>
        T* GetAt(P at)
        {
            return (T*)((uintptr_t)m_ptr.get() + (at * sizeof(T)));
        }
    };
}

