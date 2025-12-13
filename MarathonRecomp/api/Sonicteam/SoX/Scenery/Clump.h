#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/Drawable.h>
#include <Sonicteam/SoX/Graphics/InstanceModelCommon.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Scenery/CameraEventCallback.h>

namespace Sonicteam::SoX::Scenery
{
    class Clump : public Drawable
    {
    public:
        SoX::RefSharedPointer<Graphics::InstanceModelCommon> m_spInstanceModel;
        MARATHON_INSERT_PADDING(0xC);
    };

    MARATHON_ASSERT_OFFSETOF(Clump, m_spInstanceModel, 0x70);
    MARATHON_ASSERT_SIZEOF(Clump, 0x80);
}
