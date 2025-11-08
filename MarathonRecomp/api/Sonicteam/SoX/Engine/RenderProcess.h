#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/GTask.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX::Engine
{
    class RenderScheduler;

    class RenderProcess
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
        xpointer<uint32_t> m_Flag1; // or Type?
        xpointer<GTask> m_pGTask;
        be<uint32_t> m_Flag2;
        MARATHON_INSERT_PADDING(8);
        be<float> m_Field18;
        MARATHON_INSERT_PADDING(4);
        xpointer<RenderScheduler> m_pRenderScheduler;
        LinkNode<RenderProcess> m_lnRenderProcess; // TODO: verify.
    };

    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_Flag1, 0x04);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_pGTask, 0x08);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_Flag2, 0x0C);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_Field18, 0x18);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_pRenderScheduler, 0x20);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_lnRenderProcess, 0x24);
}
