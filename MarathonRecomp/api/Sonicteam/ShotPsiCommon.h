#pragma once

#include <Marathon.inl>
#include <Sonicteam/ObjectPsi.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam
{
    class ShotPsiCommon : public ObjectPsi
    {
    public:
        xpointer<void> m_pFigure;
        be<float> m_Field2C;
        SoX::Math::Vector m_Velocity;
    };

    MARATHON_ASSERT_SIZEOF(ShotPsiCommon, 0x40);
    MARATHON_ASSERT_OFFSETOF(ShotPsiCommon, m_pFigure, 0x28);
    MARATHON_ASSERT_OFFSETOF(ShotPsiCommon, m_Field2C, 0x2C);
    MARATHON_ASSERT_OFFSETOF(ShotPsiCommon, m_Velocity, 0x30);
}
