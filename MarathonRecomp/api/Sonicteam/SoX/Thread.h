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
            be<uint32_t> fpFunc04;
            be<uint32_t> fpFunc08;
            be<uint32_t> fpFunc0C;
        };

        xpointer<Vftable> m_pVftable;
        LinkNode<Thread> m_lnThread;
        be<uint32_t> m_StartEvent;
        be<uint32_t> m_EndEvent;
        be<uint32_t> m_ID;
        be<uint32_t> m_Handle;
        bool m_IsExecutable;
        MARATHON_INSERT_PADDING(3);
        be<float> m_DeltaTime;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_StepCount;
        xpointer<const char> m_pName;
        be<uint32_t> m_StepTime;
        bool m_IsThreadReady;
        bool m_IsWaitForStartEvent;
        MARATHON_INSERT_PADDING(2);
        be<uint32_t> m_WaitForEndMicroSeconds;
        xpointer<void> m_pContext;
        MARATHON_INSERT_PADDING(4);

        void Func04()
        {
            GuestToHostFunction<void>(m_pVftable->fpFunc04, this);
        }

        void Func08(float deltaTime)
        {
            GuestToHostFunction<void>(m_pVftable->fpFunc08, this, deltaTime);
        }

        template <typename T>
        T* GetContext()
        {
            return (T*)m_pContext.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(Thread, m_StartEvent, 0x10);
    MARATHON_ASSERT_OFFSETOF(Thread, m_IsThreadReady, 0x38);
}
