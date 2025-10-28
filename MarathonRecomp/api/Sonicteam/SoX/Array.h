#pragma once

#include <Marathon.h>

namespace Sonicteam::SoX
{
    /*
    for (j = 1; j >= 0; --j)
    {
        v8 += 0xFFFFFFFF;
        if (*v8)
            RefCountObjectRemoveReference(*v8); 
    }
    * if you have better name suggestion do IT */
    //be<T> or RefSharedPointer
    template <typename T, uint32_t count>
    class Array
    {
    private:
        T m_container[count];

    public:
        Array()
        {
        }
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
            return count;
        }
    };
}