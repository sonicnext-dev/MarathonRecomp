#pragma once

#include <Marathon.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam::RenderAction
{
    struct CallbackData
    {
        xpointer<DocMarathonImp> pDoc;
        xpointer<MyGraphicsDevice> pDevice;
        xpointer<RenderTargetContainer> pRenderTargetContainer;
        xpointer<SoX::Engine::RenderScheduler> pRenderScheduler;
        MARATHON_INSERT_PADDING(0x04);
        stdx::string Name;
        MARATHON_INSERT_PADDING(0x0C);
        stdx::map<stdx::string, xpointer<void>> mField3C;
    };

    MARATHON_ASSERT_OFFSETOF(CallbackData, pDoc, 0x00);
    MARATHON_ASSERT_OFFSETOF(CallbackData, pDevice, 0x04);
    MARATHON_ASSERT_OFFSETOF(CallbackData, pRenderTargetContainer, 0x08);
    MARATHON_ASSERT_OFFSETOF(CallbackData, pRenderScheduler, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CallbackData, Name, 0x14);
    MARATHON_ASSERT_OFFSETOF(CallbackData, mField3C, 0x3C);
}
