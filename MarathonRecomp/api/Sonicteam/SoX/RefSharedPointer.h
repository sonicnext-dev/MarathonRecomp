#pragma once

#include <Marathon.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX
{
    template <typename T = RefCountObject>
    class RefSharedPointer
    {
    private:
        xpointer<T> m_ptr;

    public:
        RefSharedPointer()
        {
            m_ptr = nullptr;
        }

        RefSharedPointer(T* value) : m_ptr(value)
        {
            if (m_ptr.get())
                m_ptr->AddReference();
        }

        explicit RefSharedPointer(xpointer<T> value) : m_ptr(value)
        {
            if (m_ptr.get())
                m_ptr->AddReference();
        }

        RefSharedPointer(const RefSharedPointer& other) : m_ptr(other.m_ptr)
        {
            if (m_ptr.get())
                m_ptr->AddReference();
        }

        RefSharedPointer(RefSharedPointer&& other) noexcept : m_ptr(std::move(other.m_ptr))
        {
            other.m_ptr = nullptr;
        }

        ~RefSharedPointer()
        {
            if (m_ptr.get())
                m_ptr->Release();
        }

        RefSharedPointer& operator=(const RefSharedPointer& other)
        {
            if (this != &other) 
            {
                reset();

                m_ptr = other.m_ptr;

                if (m_ptr.get())
                    m_ptr->AddReference();
            }

            return *this;
        }

        RefSharedPointer& operator=(RefSharedPointer&& other) noexcept
        {
            if (this != &other) 
            {
                reset();

                m_ptr = std::move(other.m_ptr);

                other.m_ptr = nullptr;
            }

            return *this;
        }

        void reset()
        {
            if (m_ptr.get())
                m_ptr->Release();

            m_ptr = 0;
        }

        T* get() const noexcept
        {
            return m_ptr.get();
        }

        T* operator->() const noexcept
        {
            return m_ptr.get();
        }

        T& operator*() const noexcept
        {
            return *m_ptr.get();
        }

        explicit operator bool() const noexcept
        {
            return m_ptr.get() != nullptr;
        }

        bool operator==(const RefSharedPointer& other) const noexcept
        {
            return m_ptr.get() == other.m_ptr.get();
        }

        bool operator!=(const RefSharedPointer& other) const noexcept
        {
            return !(*this == other);
        }
    };
}
