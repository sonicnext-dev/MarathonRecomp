#pragma once

#include "ppc_context.h"
#include <kernel/memory.h>
#include <kernel/heap.h>

template<typename T, bool Init = true, bool Deallocate = true>
class guest_heap_var
{
private:
    xpointer<T> m_ptr = NULL;

    void AllocHeapMemory()
    {
        m_ptr = (T*)g_userHeap.Alloc(sizeof(T));
    }

public:
    T* get()
    {
        return m_ptr.get();
    }

    const T* get() const
    {
        return (const T*)m_ptr.get();
    }

    template<typename... Args>
    guest_heap_var(Args&&... args)
    {
        AllocHeapMemory();

        if (Init)
            new (get()) T(std::forward<Args>(args)...);
    }

    guest_heap_var(const guest_heap_var<T>& other)
    {
        AllocHeapMemory();

        if (Init)
            new (get()) T(*other->get());
    }

    //
    guest_heap_var(guest_heap_var<T>&& other)
    {
        m_ptr = other.m_ptr;
        other.m_ptr = 0;
    }

    ~guest_heap_var()
    {
        if (get() && Deallocate)
        {
            get()->~T();
            g_userHeap.Free(get());
        }
    }

    void operator=(const guest_heap_var<T>& other)
    {
        if (this != &other)
            *get() = *other->get();
    }

    void operator=(guest_heap_var<T>&& other)
    {
        if (this != &other)
        {

            if (get() && Deallocate)
            {
                get()->~T();
                g_userHeap.Free(get());
            }

            m_ptr = other.m_ptr;
            other.m_ptr = NULL;
        }
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
