#pragma once

#include <Marathon.h>

namespace Sonicteam::SoX
{
    template <typename T, uint32_t Length>
    class Array
    {
    private:
        T m_container[Length];

    public:
        Array() {}
        ~Array() = default;

        T& operator[](uint32_t index)
        {
            return m_container[index];
        }

        const T& operator[](uint32_t index) const
        {
            return m_container[index];
        }

        uint32_t size() const
        {
            return Length;
        }
    };
}
