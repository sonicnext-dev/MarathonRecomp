#pragma once

#include <Marathon.inl>
#include <stdx/list.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Engine/GTask.h>

namespace Sonicteam::SoX::Engine
{
    class RenderScheduler;

    class RenderProcess
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
        xpointer<uint32_t> m_Flag1; //or Type?
        xpointer<GTask> m_pGTask;
        be<uint32_t> m_Flag2;
        MARATHON_INSERT_PADDING(0x8);
        be<float> m_Field18;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<RenderScheduler> m_pRenderScheduler;
        LinkNode<RenderProcess> m_lnRenderProcess; // ?? i guess ?? (check pls)
    };

    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_Flag1, 4);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_pGTask, 8);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_Flag2, 0xC);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_Field18, 0x18);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_pRenderScheduler, 0x20);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_lnRenderProcess, 0x24);
}
