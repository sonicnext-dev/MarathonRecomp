#pragma once
#include <Marathon.inl>
#include <Sonicteam/Player/State/ICommonContext.h>
#include <Sonicteam/Player/IExportPostureRequestFlag.h>
#include <Sonicteam/Player/IExportWeaponRequestFlag.h>
#include <Sonicteam/Player/Score.h>
#include <api/boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Player::State
{
    class CommonContext : public ICommonContext, public IExportPostureRequestFlag, public IExportWeaponRequestFlag
    {
    public:
        xpointer<void> m_spComboAttackManager;
        be<uint32_t> m_ContextIFFlag1;
        be<uint32_t> m_ContextIFFlag2;
        be<uint32_t> m_ContextIFFlag3;
        be<uint32_t> m_ExportPostureRequestFlag;
        be<uint32_t> m_ExportWeaponRequestFlag;
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_PostureFlag;
        be<uint64_t> m_ExternalFlag;
        be<uint32_t> m_VehicleFlag;
        be<uint32_t> m_AmigoFlag;
        MARATHON_INSERT_PADDING(0x50);
        boost::shared_ptr<Score> m_pScore; //on merge, rename to spScore also do not forget about player_patches.cpp
        MARATHON_INSERT_PADDING(0x100);
    };
}
