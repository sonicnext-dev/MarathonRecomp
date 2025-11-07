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
            be<uint32_t> fpFunc4;
            be<uint32_t> fpFunc8;
            be<uint32_t> fpFuncC;
        };

        xpointer<Vftable> m_pVftable;
        LinkNode<Thread> m_lnThread;
        be<uint32_t> m_StartEvent;
        be<uint32_t> m_EndEvent;
        be<uint32_t> m_ID;
        be<uint32_t> m_Handle;
        bool m_IsExecutable; //  while ( a1->IsExecutable ) ref to 82586C00
        MARATHON_INSERT_PADDING(3);
        be<float> m_DeltaTime;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_StepCount;
        xpointer<const char> m_pName;
        be<uint32_t> m_StepTime; //ref to 82586A08
        bool m_IsThreadReady;
        bool m_IsWaitForStartEvent; //ref to 82586C00
        MARATHON_INSERT_PADDING(2);
        be<uint32_t> m_WaitForEndMicroSeconds; //82586AC0, BaseTimeAfter - BaseTime(Before WaitForEndEvent), then / 1000000 (TICK_PER_SEC )
        xpointer<void> m_pContext;
        MARATHON_INSERT_PADDING(8);


        void Func4()
        {
            GuestToHostFunction<void>(m_pVftable->fpFunc4, this);
        }

        void Func8(float delta)
        {
            GuestToHostFunction<void>(m_pVftable->fpFunc8, this, delta);
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
