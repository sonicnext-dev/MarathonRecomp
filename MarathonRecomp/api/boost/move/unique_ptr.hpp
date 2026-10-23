#pragma once

#include <Marathon.inl>
#include <kernel/heap.h>

namespace boost
{
    template <typename T>
    class unique_ptr
    {
    public:
        xpointer<std::decay_t<T>> p;

        T* get()
        {
            return p.get();
        }

        const T* get() const
        {
            return reinterpret_cast<const T*>(p.get());
        }

        ~unique_ptr()
        {
            g_userHeap.Free(reinterpret_cast<void*>(p.get()));
        }

        void operator=(const unique_ptr<T>& other)
        {
            if (this != &other)
                *get() = *other->get();
        }

        void operator=(const T& other)
        {
            if (get() != &other)
                *get() = *other;
        }

        void operator=(T&& other)
        {
            if (get() != &other)
                *get() = std::move(*other);
        }

        operator const T* () const
        {
            return get();
        }

        operator T* ()
        {
            return get();
        }

        const T* operator->() const
        {
            return get();
        }

        T* operator->()
        {
            return get();
        }

        const T& operator*() const
        {
            return *get();
        }

        T& operator*()
        {
            return *get();
        }
    };
}
