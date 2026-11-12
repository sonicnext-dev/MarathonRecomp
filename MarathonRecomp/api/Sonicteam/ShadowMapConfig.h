#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam
{
    struct ShadowMapConfig
    {
        be<float> Distance;
        be<float> Near;
        be<float> Far;
        be<float> CameraDistance;
        SoX::Math::Vector CenterOffset;
    };

    MARATHON_ASSERT_OFFSETOF(ShadowMapConfig, Distance, 0x00);
    MARATHON_ASSERT_OFFSETOF(ShadowMapConfig, Near, 0x04);
    MARATHON_ASSERT_OFFSETOF(ShadowMapConfig, Far, 0x08);
    MARATHON_ASSERT_OFFSETOF(ShadowMapConfig, CameraDistance, 0x0C);
    MARATHON_ASSERT_OFFSETOF(ShadowMapConfig, CenterOffset, 0x10);
    MARATHON_ASSERT_SIZEOF(ShadowMapConfig, 0x20);
}
