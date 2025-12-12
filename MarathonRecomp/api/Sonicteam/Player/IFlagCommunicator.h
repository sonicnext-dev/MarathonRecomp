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
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpDestroy, 0);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportICommonContextIFFlag, 4);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportIPostureControlFlag, 8);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpFunc0xC, 0xC);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportExternalFlag, 0x10);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportICommonContextIFFlag2, 0x14);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportICommonContextIFFlag3, 0x18);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportIModelAnimationFlag, 0x1C);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportPostureRequestFlag, 0x20);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportWeaponRequestFlag, 0x24);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportUpgradeFlag, 0x28);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportVehicleFlag, 0x2C);
    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator::Vftable, fpImportAIIBaseFlag, 0x30);
    MARATHON_ASSERT_SIZEOF(IFlagCommunicator, 4);

}
