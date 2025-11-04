#pragma once

#include "stdx/string.h"

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/GTask.h>

namespace Sonicteam::SoX::Engine
{
    class RenderProcess
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x10);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<GTask> m_pGTask;
        MARATHON_INSERT_PADDING(0x8);
        xpointer<stdx::string> m_pCurrentCamera;
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_SIZEOF(RenderProcess, 0x30);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_pGTask, 0x8);
    MARATHON_ASSERT_OFFSETOF(RenderProcess, m_pCurrentCamera, 0x14);
}
