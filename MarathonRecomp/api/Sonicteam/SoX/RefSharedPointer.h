#pragma once

#include <Marathon.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX
{
    template <typename RefType = RefCountObject>
    class RefSharedPointer
    {
    private:
        xpointer<RefType> m_value;

    public:
        explicit RefSharedPointer(RefType* value)
            : m_value(value)
        {
            if (m_value.get()) 
            {
                m_value->AddRef();
            }
        }

        explicit RefSharedPointer(xpointer<RefType> value)
            : m_value(value)
        {
            if (m_value.get()) 
            {
                m_value->AddRef();
            }
        }

        RefSharedPointer(const RefSharedPointer& other)
            : m_value(other.m_value)
        {
            if (m_value.get()) 
            {
                m_value->AddRef();
            }
        }

        RefSharedPointer(RefSharedPointer&& other) noexcept
            : m_value(std::move(other.m_value))
        {
            other.m_value = nullptr;
        }

        ~RefSharedPointer()
        {
            if (m_value.get())
            {
                m_value->Release();
            }
        }

        RefSharedPointer& operator=(const RefSharedPointer& other)
        {
            if (this != &other) 
            {
                reset();
                m_value = other.m_value;

                if (m_value.get()) 
                {
                    m_value->AddRef();
                }
            }
            return *this;
        }

        RefSharedPointer& operator=(RefSharedPointer&& other) noexcept
        {
            if (this != &other) 
            {
                reset();

                m_value = std::move(other.m_value);
                other.m_value = nullptr;
            }
            return *this;
        }

        void reset()
        {
            if (m_value.get())
            {
                m_value->Release();
            }
            m_value = 0;
        }

        RefType* get() const noexcept
        {
            return m_value.get();
        }

        RefType* operator->() const noexcept
        {
            return m_value.get();
        }

        RefType& operator*() const noexcept
        {
            return *m_value.get();
        }

        explicit operator bool() const noexcept
        {
            return m_value.get() != nullptr;
        }

        bool operator==(const RefSharedPointer& other) const noexcept
        {
            return m_value.get() == other.m_value.get();
        }

        bool operator!=(const RefSharedPointer& other) const noexcept
        {
            return !(*this == other);
        }
    };
}
