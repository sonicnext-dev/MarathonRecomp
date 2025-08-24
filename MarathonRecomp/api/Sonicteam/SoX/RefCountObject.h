#pragma once

#include <Marathon.h>

namespace Sonicteam::SoX
{
    class RefCountObject
    {
    public:
        xpointer<void> m_pVftable;
        be<uint32_t> m_ReferenceCount;

        void Release(uint32_t Flag = 1)
        {
            m_ReferenceCount = m_ReferenceCount - 1;
            if (!m_ReferenceCount.get())
                MARATHON_VIRTUAL_FUNCTION(void*, 0,this, Flag);
        }

        inline void AddRef()
        {
            m_ReferenceCount = m_ReferenceCount + 1;
        }

    };
}
