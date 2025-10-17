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
        be<uint32_t> m_CommonContextIFFlags1;
        be<uint32_t> m_CommonContextIFFlags2;
        be<uint32_t> m_CommonContextIFFlags3;
        be<uint32_t> m_ExportPostureRequestFlags;
        be<uint32_t> m_ExportWeaponRequestFlags;
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_PostureFlags;
        be<uint64_t> m_ExternalFlags;
        be<uint32_t> m_VehicleFlags;
        be<uint32_t> m_AmigoFlags;
        MARATHON_INSERT_PADDING(0x50);
        boost::shared_ptr<Score> m_spScore;
        MARATHON_INSERT_PADDING(8);
        boost::shared_ptr<Input::ListenerNormal> m_spInputListenerNormal;
        boost::anonymous_shared_ptr m_spInputListenerAmigo;
        MARATHON_INSERT_PADDING(0xE8);

        SoX::Input::Manager* GetInputManager() const
        {
            if (!m_spScore.get())
                return nullptr;

            auto pPlayer = m_spScore->m_pPlayer;
            
            return pPlayer->GetDoc<DocMarathonImp>()->m_vspInputManager[pPlayer->m_ControllerIndex].get();
        }
    };
}
