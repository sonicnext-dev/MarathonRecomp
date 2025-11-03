#pragma once

#include <Marathon.inl>
#include <Sonicteam/GE1PE/RefCountObject.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam::GE1PE
{
    //.peb

    class ParticleEngine;

    //no name left for type
    class EffectBankData
    {
    public:
        be<uint32_t> m_Field0; // 0x3E - no idea
        stdx::string m_DataName; // player_sonic
        //from .peb (raw converted since they released after)
        xpointer<void> m_Data1;
        xpointer<void> m_Data2;
        xpointer<void> m_Data3;
        stdx::map<stdx::string, be<uint32_t>> m_EffectDataIndices; //barrier_g = 0x23
    };
    MARATHON_ASSERT_OFFSETOF(EffectBankData, m_EffectDataIndices, 0x2C);
    MARATHON_ASSERT_SIZEOF(EffectBankData, 0x38);

    class EffectBank : public RefCountObject
    {
    public:
        xpointer<ParticleEngine> m_pParticleEngine;
        be<uint32_t> m_FieldC; //index ?
        xpointer<EffectBankData> m_pEffectBankA;
    };
    MARATHON_ASSERT_SIZEOF(EffectBank, 0x14);
}
