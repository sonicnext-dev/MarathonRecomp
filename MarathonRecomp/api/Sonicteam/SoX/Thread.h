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
        MARATHON_INSERT_PADDING(4);

        template <typename T>
        T* GetContext()
        {
            return (T*)m_pContext.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(Thread, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(Thread, m_pParent, 0x0C);
    MARATHON_ASSERT_OFFSETOF(Thread, m_EventHandleA, 0x10);
    MARATHON_ASSERT_OFFSETOF(Thread, m_EventHandleB, 0x14);
    MARATHON_ASSERT_OFFSETOF(Thread, m_ThreadHandle, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Thread, m_Field20, 0x20);
    MARATHON_ASSERT_OFFSETOF(Thread, m_DeltaTime, 0x24);
    MARATHON_ASSERT_OFFSETOF(Thread, m_pName, 0x30);
    MARATHON_ASSERT_OFFSETOF(Thread, m_Field38, 0x38);
    MARATHON_ASSERT_OFFSETOF(Thread, m_Field39, 0x39);
    MARATHON_ASSERT_OFFSETOF(Thread, m_pContext, 0x40);
    MARATHON_ASSERT_SIZEOF(Thread, 0x48);
}
