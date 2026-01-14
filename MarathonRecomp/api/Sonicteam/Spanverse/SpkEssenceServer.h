#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkBase.h>
#include <Sonicteam/Spanverse/SpkEssenceBase.h>

namespace Sonicteam::Spanverse
{
    class SpkEssenceServer : public SpkBase
    {
    public:

        enum SpkEssenceBase
        {
            SpkEssenceBase_CustomEssence = 0x4E584946, // NXIF
            SpkEssenceBase_CustomEssenceTextureDDM = 0x44444D20, // DDM
            SpkEssenceBase_CustomEssenceTextureDDS = 0x44445320, // DDS
            SpkEssenceBase_CustomEssenceNeParticle = 0x50454230, // PEB0
            SpkEssenceBase_CustomEssenceAcroarts = 0x4D524142, // MRAB
            SpkEssenceBase_CustomEssenceCellSprite = 0x46415043 // FAPC
        };

        static constexpr uint32_t ms_Signature = 0x53534553; // SSES
        xpointer<SpkEssenceBase> m_paSpkEssenceBase[5]; //NXIF, DDM, DDS, PEB0(CustomEssenceNeParticle), MRAB(CustomEssenceAcroart), FAPC(CustomEssenceCellSprite)

    };

    MARATHON_ASSERT_OFFSETOF(SpkEssenceServer, m_paSpkEssenceBase, 0x10);
}
