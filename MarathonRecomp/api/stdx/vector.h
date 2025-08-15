#pragma once

#include <kernel/heap.h>
#include <kernel/function.h>

namespace stdx
{
    template <typename Type>
    struct vector
    {
        be<uint32_t> _MyProxy;
        xpointer<Type> _MyFirst;
        xpointer<Type> _MyLast;
        xpointer<Type> _MyEnd;

        Type& operator[](uint32_t index)
        {
            return *(Type*)((uint64_t)_MyFirst.get() + (index * sizeof(Type)));
        }
        size_t size()
        {
            return (_MyLast.ptr.get() - _MyFirst.ptr.get()) / sizeof(Type);
        }
    };
}