#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IFlagCommunicator
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpImportICommonContextIFFlag;
            be<uint32_t> fpImportIPostureControlFlag;
            be<uint32_t> fpFunc0xC;
            be<uint32_t> fpImportExternalFlag;
            be<uint32_t> fpImportICommonContextIFFlag2;
            be<uint32_t> fpImportICommonContextIFFlag3;
            be<uint32_t> fpImportIModelAnimationFlag;
            be<uint32_t> fpImportPostureRequestFlag;
            be<uint32_t> fpImportWeaponRequestFlag;
            be<uint32_t> fpImportUpgradeFlag;
            be<uint32_t> fpImportVehicleFlag;
            be<uint32_t> fpImportAIIBaseFlag;
        };

        xpointer<Vftable> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IFlagCommunicator, 4);
}
