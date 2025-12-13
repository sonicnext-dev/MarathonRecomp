#pragma once

#include <Marathon.inl>
#include <Sonicteam/GE1PE/RefCountObject.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam::GE1PE
{
    class ParticleEngine;

    struct EffectBankData
    {
        be<uint32_t> Field00;
        stdx::string Name;
        xpointer<void> pData1;
        xpointer<void> pData2;
        xpointer<void> pData3;
        stdx::map<stdx::string, be<uint32_t>> mEffectDataIndices;
    };

    class EffectBank : public RefCountObject
    {
    public:
        xpointer<ParticleEngine> m_pParticleEngine;
        be<uint32_t> m_Field0C; // Index?
        xpointer<EffectBankData> m_pEffectBankA;
    };

    MARATHON_ASSERT_OFFSETOF(EffectBankData, Field00, 0x00);
    MARATHON_ASSERT_OFFSETOF(EffectBankData, Name, 0x04);
    MARATHON_ASSERT_OFFSETOF(EffectBankData, pData1, 0x20);
    MARATHON_ASSERT_OFFSETOF(EffectBankData, pData2, 0x24);
    MARATHON_ASSERT_OFFSETOF(EffectBankData, pData3, 0x28);
    MARATHON_ASSERT_OFFSETOF(EffectBankData, mEffectDataIndices, 0x2C);
    MARATHON_ASSERT_SIZEOF(EffectBankData, 0x38);

    MARATHON_ASSERT_OFFSETOF(EffectBank, m_pParticleEngine, 0x08);
    MARATHON_ASSERT_OFFSETOF(EffectBank, m_Field0C, 0x0C);
    MARATHON_ASSERT_OFFSETOF(EffectBank, m_pEffectBankA, 0x10);
    MARATHON_ASSERT_SIZEOF(EffectBank, 0x14);
}
