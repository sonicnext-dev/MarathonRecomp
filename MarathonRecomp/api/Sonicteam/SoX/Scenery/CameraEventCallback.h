#pragma once

#include <Marathon.h>

namespace Sonicteam::SoX::Scenery
{
    class CameraEventCallback
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(CameraEventCallback, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(CameraEventCallback, 4);
}
