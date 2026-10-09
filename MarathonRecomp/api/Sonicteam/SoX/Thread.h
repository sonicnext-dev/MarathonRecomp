#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX
{
    class Thread
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpAwait;
            be<uint32_t> fpStart;
            be<uint32_t> fpStep;
        };

        xpointer<Vftable> m_pVftable;
        LinkNode<Thread> m_lnThread;
        be<uint32_t> m_StartEvent;
        be<uint32_t> m_EndEvent;
        be<uint32_t> m_ID;
        be<uint32_t> m_Handle;
        bool m_IsExecutable;
        MARATHON_INSERT_PADDING(0x03);
        be<float> m_DeltaTime;
        MARATHON_INSERT_PADDING(0x04);
        be<uint32_t> m_StepCount;
        xpointer<const char> m_pName;
        be<uint32_t> m_StepTime;
        bool m_IsThreadReady;
        bool m_UseEvents;
        MARATHON_INSERT_PADDING(0x02);
        be<uint32_t> m_EndTime;
        xpointer<void> m_pContext;
        MARATHON_INSERT_PADDING(0x08);

        uint64_t Await()
        {
            return GuestToHostFunction<uint64_t>(m_pVftable->fpAwait, this);
        }

        void Start()
        {
            GuestToHostFunction<void>(m_pVftable->fpStart, this);
        }

        void Step(double deltaTime)
        {
            GuestToHostFunction<void>(m_pVftable->fpStep, this, deltaTime);
        }

        template <typename T>
        T* GetContext()
        {
            return (T*)m_pContext.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(Thread, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(Thread, m_lnThread, 0x04);
    MARATHON_ASSERT_OFFSETOF(Thread, m_StartEvent, 0x10);
    MARATHON_ASSERT_OFFSETOF(Thread, m_EndEvent, 0x14);
    MARATHON_ASSERT_OFFSETOF(Thread, m_ID, 0x18);
    MARATHON_ASSERT_OFFSETOF(Thread, m_Handle, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Thread, m_IsExecutable, 0x20);
    MARATHON_ASSERT_OFFSETOF(Thread, m_DeltaTime, 0x24);
    MARATHON_ASSERT_OFFSETOF(Thread, m_StepCount, 0x2C);
    MARATHON_ASSERT_OFFSETOF(Thread, m_pName, 0x30);
    MARATHON_ASSERT_OFFSETOF(Thread, m_StepTime, 0x34);
    MARATHON_ASSERT_OFFSETOF(Thread, m_IsThreadReady, 0x38);
    MARATHON_ASSERT_OFFSETOF(Thread, m_UseEvents, 0x39);
    MARATHON_ASSERT_OFFSETOF(Thread, m_EndTime, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Thread, m_pContext, 0x40);
    MARATHON_ASSERT_SIZEOF(Thread, 0x4C);
}
