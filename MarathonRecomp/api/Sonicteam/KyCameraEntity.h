#pragma once

#include <Marathon.inl>
#include <Kaim/CEntity.h>
#include <Kaim/CEntityEyePosition.h>
#include <Kaim/CEntityHeadDirection.h>
#include <Kaim/CEntityMaxSpeed.h>
#include <Kaim/CEntityVisualAcuteness.h>
#include <Kaim/CEntityWidth.h>
#include <Sonicteam/KynapseSkel/CAiMeshEntityAttribute.h>

namespace Sonicteam
{
    class KyCameraEntity : public Kaim::CEntity
    {
    public:
        xpointer<Kaim::CEntityMaxSpeed> m_pMaxSpeed;
        xpointer<Kaim::CEntityWidth> m_pWidth;
        xpointer<Kaim::CEntityHeadDirection> m_pHeadDirection;
        xpointer<Kaim::CEntityEyePosition> m_pEyePosition;
        xpointer<Kaim::CEntityVisualAcuteness> m_pVisualAcuteness;
        xpointer<KynapseSkel::CAiMeshEntityAttribute> m_pAiMesh;
    };

    MARATHON_ASSERT_OFFSETOF(KyCameraEntity, m_pMaxSpeed, 0x70);
    MARATHON_ASSERT_OFFSETOF(KyCameraEntity, m_pWidth, 0x74);
    MARATHON_ASSERT_OFFSETOF(KyCameraEntity, m_pHeadDirection, 0x78);
    MARATHON_ASSERT_OFFSETOF(KyCameraEntity, m_pEyePosition, 0x7C);
    MARATHON_ASSERT_OFFSETOF(KyCameraEntity, m_pVisualAcuteness, 0x80);
    MARATHON_ASSERT_OFFSETOF(KyCameraEntity, m_pAiMesh, 0x84);
    MARATHON_ASSERT_SIZEOF(KyCameraEntity, 0x88);
}
