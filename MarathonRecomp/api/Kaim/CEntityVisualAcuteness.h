#pragma once

#include <Marathon.inl>
#include <Kaim/CEntityAttribute.h>

namespace Kaim
{
    class CEntityVisualAcuteness : public CEntityAttribute
    {
    public:
        be<float> m_visibilityDistance;
        be<float> m_visibilityAngle;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityVisualAcuteness, m_visibilityDistance, 0x04);
    MARATHON_ASSERT_OFFSETOF(CEntityVisualAcuteness, m_visibilityAngle, 0x08);
    MARATHON_ASSERT_SIZEOF(CEntityVisualAcuteness, 0x0C);
}
