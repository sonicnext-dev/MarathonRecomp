#pragma once

#include <Marathon.h>
#include <kernel/heap.h>

namespace Sonicteam::SoX
{
    // add support for custom destructor function
    template <typename T>
    class ArrayOld
    {
    private:
        struct Header
        {
            be<uint32_t> m_count;
            T m_elements[0];
        };

        xpointer<T> m_elements;

    public:
        // Iterator types
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        ArrayOld()
        {

        }

        ArrayOld(uint32_t count)
        {
            size_t size = sizeof(Header) + sizeof(T) * count;
            Header* header = (Header*)g_userHeap.Alloc(size);
            header->m_count = count;

            m_elements = reinterpret_cast<T*>(header + 1);

            for (uint32_t i = 0; i < count; ++i)
            {
                new (&m_elements[i]) T();
            }
        }

        // Element access
        T* data()
        {
            return m_elements;
        }

        const T* data() const
        {
            return m_elements;
        }

        T& operator[](size_t index)
        {
            return m_elements[index];
        }

        const T& operator[](size_t index) const
        {
            return m_elements[index];
        }

        uint32_t size() const
        {

            Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(m_elements.get()) - sizeof(Header));
            return header->m_count;
        }

        bool empty() const
        {
            return size() == 0;
        }

        // Iterators
        iterator begin()
        {
            return m_elements;
        }

        iterator end()
        {
            return m_elements + size();
        }

        const_iterator begin() const
        {
            return m_elements;
        }

        const_iterator end() const
        {
            return m_elements + size();
        }

        const_iterator cbegin() const
        {
            return m_elements;
        }

        const_iterator cend() const
        {
            return m_elements + size();
        }

        // Reverse iterators
        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }

        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crbegin() const
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crend() const
        {
            return const_reverse_iterator(begin());
        }

        ~ArrayOld()
        {
            if (!m_elements) return;

            Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(m_elements.get()) - sizeof(Header));
            uint32_t count = header->m_count;

            // Destroy elements backwards
            for (int32_t i = count - 1; i >= 0; --i)
            {
                m_elements[i].~T();
            }

            g_userHeap.Free(header);
        }
    };


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

    template <typename T>
    struct ArrayPtr {
        xpointer<T> pitems;
        uint32_t count;

        ArrayPtr() : pitems(NULL), count(0) 
        {
        }

        explicit ArrayPtr(uint32_t size) :
            pitems(g_userHeap.Alloc(size * sizeof(T))),  
            count(size) 
        {
        }


        ArrayPtr(T* ptr, uint32_t size)
            : pitems(ptr), count(size) 
        {
        }

        T& operator[](size_t index) 
        {
            return pitems[index];
        }

        ArrayPtr(const ArrayPtr&);
        ArrayPtr& operator=(const ArrayPtr&);
    };

    template <typename T>
    struct AIMArray 
    {
        xpointer<T> pitems; 
        uint32_t LastItemID; 
        uint32_t EndItemID;

        explicit AIMArray(uint32_t size)
            : pitems(g_userHeap.Alloc(size * sizeof(T))),
            LastItemID(-1),
            EndItemID(size - 1) 
        {
        }

        AIMArray(T* existingItems, uint32_t size)
            : pitems(existingItems), LastItemID(size - 1), EndItemID(size - 1)
        {
        }

        ~AIMArray() 
        {
            g_userHeap.Free(pitems);
            pitems = 0;
        }

    private:
        AIMArray(const AIMArray&);
        AIMArray& operator=(const AIMArray&);

    public:

        T& at(size_t index) {
            return pitems[index];
        }

        const T& at(size_t index) const {
            return pitems[index];
        }
    };

}
