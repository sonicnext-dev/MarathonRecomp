#pragma once

#include <Marathon.inl>
#include <Sonicteam/ObjectPsi.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam
{
    class EnemyPsiCommon : public ObjectPsi
    {
    public:
        xpointer<void> m_pFigure;
        be<float> m_Field2C;
        SoX::Math::Vector m_Velocity;
        SoX::Math::Vector m_Rotation;
    };

    MARATHON_ASSERT_SIZEOF(EnemyPsiCommon, 0x50);
    MARATHON_ASSERT_OFFSETOF(EnemyPsiCommon, m_pFigure, 0x28);
    MARATHON_ASSERT_OFFSETOF(EnemyPsiCommon, m_Field2C, 0x2C);
    MARATHON_ASSERT_OFFSETOF(EnemyPsiCommon, m_Velocity, 0x30);
    MARATHON_ASSERT_OFFSETOF(EnemyPsiCommon, m_Rotation, 0x40);
}
