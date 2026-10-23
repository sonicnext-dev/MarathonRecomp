#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdColor.h>
#include <Chao/CSD/Core/csdVector.h>
#include <Chao/CSD/Manager/csdmResourceBase.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Chao::CSD
{
    struct MotionPattern
    {
        MARATHON_INSERT_PADDING(0x28);
        be<int> HideFlag;
        Chao::CSD::Vector2 Position;
        be<float> Rotation;
        Chao::CSD::Vector2 Scale;
        be<float> PatternIndexFlags;
        Chao::CSD::Color Color;
        Chao::CSD::Color TopLeft;
        Chao::CSD::Color BottomLeft;
        Chao::CSD::Color TopRight;
        Chao::CSD::Color BottomRight;
    };

    class CMotionPattern : public CResourceBase<MotionPattern>, CBase
    {
    public:
        MARATHON_INSERT_PADDING(0x2C);

        int GetPatternIndex()
        {
            return int(m_pResource->PatternIndexFlags.get()) % 10000;
        }
    };

    MARATHON_ASSERT_OFFSETOF(MotionPattern, HideFlag, 0x28);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, Position, 0x2C);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, Rotation, 0x34);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, Scale, 0x38);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, PatternIndexFlags, 0x40);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, Color, 0x44);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, TopLeft, 0x48);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, BottomLeft, 0x4C);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, TopRight, 0x50);
    MARATHON_ASSERT_OFFSETOF(MotionPattern, BottomRight, 0x54);

    MARATHON_ASSERT_SIZEOF(CMotionPattern, 0x38);
}
