#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX
{
    class Thread
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(8);
        xpointer<Thread> m_pParent;
        be<uint32_t> m_EventHandleA;
        be<uint32_t> m_EventHandleB;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_ThreadHandle;
        bool m_Field20;
        MARATHON_INSERT_PADDING(3);
        be<float> m_DeltaTime;
        MARATHON_INSERT_PADDING(8);
        xpointer<const char> m_pName;
        MARATHON_INSERT_PADDING(4);
        bool m_Field38;
        bool m_Field39;
        MARATHON_INSERT_PADDING(6);
        xpointer<void> m_pContext;
        MARATHON_INSERT_PADDING(8);

        template <typename T>
        T* GetContext()
        {
            return (T*)m_pContext.get();
        }
    };
}
