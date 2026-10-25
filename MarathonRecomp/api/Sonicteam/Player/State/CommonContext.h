#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Player/State/CommonMiscFlagsSlot.h>
#include <Sonicteam/Player/State/ICommonContext.h>
#include <Sonicteam/Player/IExportPostureRequestFlag.h>
#include <Sonicteam/Player/IExportWeaponRequestFlag.h>
#include <Sonicteam/Player/PostureControlFlags.h>
#include <Sonicteam/Player/Score.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::Combo
{
    class AttackManager;
}

namespace Sonicteam::Player::State
{
    class CommonContext : public ICommonContext, public IExportPostureRequestFlag, public IExportWeaponRequestFlag
    {
    public:
        SoX::RefSharedPointer<Combo::AttackManager> m_spAttackManager;
        be<uint32_t> m_CommonContextIFFlags1;
        be<uint32_t> m_CommonContextIFFlags2;
        be<uint32_t> m_CommonContextIFFlags3;
        be<uint32_t> m_ExportPostureRequestFlags;
        be<uint32_t> m_ExportWeaponRequestFlags;
        MARATHON_INSERT_PADDING(0x14);
        be<PostureControlFlags> m_PostureFlags;
        be<uint64_t> m_ExternalFlags;
        be<uint32_t> m_VehicleFlags;
        be<uint32_t> m_AmigoFlags;
        uint8_t m_MiscFlags[80];
        boost::shared_ptr<Score> m_spScore;
        MARATHON_INSERT_PADDING(8);
        boost::shared_ptr<Input::ListenerNormal> m_spInputListenerNormal;
        boost::anonymous_shared_ptr m_spInputListenerAmigo;
        MARATHON_INSERT_PADDING(0xE8);

        uint8_t GetMiscFlags(CommonMiscFlagsSlot in_slot) const
        {
            if (in_slot >= 80)
                return 0;

            return m_MiscFlags[in_slot];
        }

        SoX::Input::Manager* GetInputManager() const
        {
            if (!m_spScore.get())
                return nullptr;

            return m_spScore->m_pPlayer->GetInputManager();
        }
    };

    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spAttackManager, 0x98);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_CommonContextIFFlags1, 0x9C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_CommonContextIFFlags2, 0xA0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_CommonContextIFFlags3, 0xA4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_ExportPostureRequestFlags, 0xA8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_ExportWeaponRequestFlags, 0xAC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_PostureFlags, 0xC4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_ExternalFlags, 0xC8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_VehicleFlags, 0xD0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_AmigoFlags, 0xD4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_MiscFlags, 0xD8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spScore, 0x128);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spInputListenerNormal, 0x138);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spInputListenerAmigo, 0x140);
    MARATHON_ASSERT_SIZEOF(CommonContext, 0x230);
}
