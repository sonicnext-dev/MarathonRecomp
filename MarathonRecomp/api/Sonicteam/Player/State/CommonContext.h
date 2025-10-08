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
        be<float> m_Field148;
        be<float> gravity; // Lua parameter (no "c_" prefix)
        be<float> c_brake_acc;
        be<float> c_brake_dashpanel;
        be<float> c_walk_border;
        be<float> c_run_border;
        be<float> c_walk_speed_max;
        be<float> c_run_acc;
        be<float> c_run_speed_max;
        be<float> c_speedup_acc;
        be<float> c_speedup_speed_max;
        be<float> c_jump_time_min;
        be<float> c_jump_brake;
        be<float> c_jump_speed_acc;
        be<float> c_jump_speed_brake;
        be<float> c_jump_speed;
        be<float> c_jump_walk;
        be<float> c_jump_run;
        be<float> c_brake_quick_acc;
        be<float> c_wait_no_input_time;
        be<float> c_damage_time;
        be<float> c_damage_jump;
        be<float> c_damage_speed;
        be<float> c_run_against_time;
        be<float> c_grind_speed_org;
        be<float> c_grind_acc;
        be<float> c_grind_speed_max;
        be<float> c_grind_time;
        be<float> c_grind_penalty_time;
        be<float> c_grind_brake_acc;
        be<float> c_invincible_time;
        be<float> c_invincible_time_ring1;
        be<float> c_invincible_item;
        be<float> c_speedup_time;
        be<float> c_wind_init;
        be<float> c_wind_spd;
        be<float> c_wind_dist;
        be<float> c_border_gravity;
        be<float> c_landing_time;
        be<float> c_ottoto_time;
        be<float> c_dead_animation_time;
        be<float> c_dead_animation_time_coll;
        be<float> c_wallwait_time;
        be<float> c_lclick_time;
        be<float> c_flight_acc;
        be<float> c_flight_speed_acc;
        be<float> c_flight_speed_min;
        be<float> c_flight_speed_max;
        be<float> c_hovering_acc;
        be<float> c_climb_speed;
        be<float> c_stun;
        be<float> c_brake_acc_sand;
        be<float> c_run_acc_sand;
        be<float> c_jump_speed_sand;
        be<float> c_psi_throw_speed;
        be<float> m_Field224;
        be<float> m_Field228;
        be<float> m_Field22C;

        SoX::Input::Manager* GetInputManager() const
        {
            if (!m_spScore.get())
                return nullptr;

            return m_spScore->m_pPlayer->GetInputManager();
        }
    };

    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spComboAttackManager, 0x98);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_CommonContextIFFlags1, 0x9C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_CommonContextIFFlags2, 0xA0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_CommonContextIFFlags3, 0xA4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_ExportPostureRequestFlags, 0xA8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_ExportWeaponRequestFlags, 0xAC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_PostureFlags, 0xC4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_ExternalFlags, 0xC8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_VehicleFlags, 0xD0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_AmigoFlags, 0xD4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spScore, 0x128);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spInputListenerNormal, 0x138);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_spInputListenerAmigo, 0x140);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_Field148, 0x148);
    MARATHON_ASSERT_OFFSETOF(CommonContext, gravity, 0x14C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_brake_acc, 0x150);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_brake_dashpanel, 0x154);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_walk_border, 0x158);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_run_border, 0x15C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_walk_speed_max, 0x160);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_run_acc, 0x164);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_run_speed_max, 0x168);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_speedup_acc, 0x16C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_speedup_speed_max, 0x170);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_time_min, 0x174);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_brake, 0x178);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_speed_acc, 0x17C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_speed_brake, 0x180);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_speed, 0x184);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_walk, 0x188);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_run, 0x18C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_brake_quick_acc, 0x190);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_wait_no_input_time, 0x194);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_damage_time, 0x198);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_damage_jump, 0x19C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_damage_speed, 0x1A0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_run_against_time, 0x1A4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_grind_speed_org, 0x1A8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_grind_acc, 0x1AC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_grind_speed_max, 0x1B0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_grind_time, 0x1B4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_grind_penalty_time, 0x1B8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_grind_brake_acc, 0x1BC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_invincible_time, 0x1C0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_invincible_time_ring1, 0x1C4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_invincible_item, 0x1C8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_speedup_time, 0x1CC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_wind_init, 0x1D0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_wind_spd, 0x1D4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_wind_dist, 0x1D8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_border_gravity, 0x1DC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_landing_time, 0x1E0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_ottoto_time, 0x1E4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_dead_animation_time, 0x1E8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_dead_animation_time_coll, 0x1EC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_wallwait_time, 0x1F0);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_lclick_time, 0x1F4);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_flight_acc, 0x1F8);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_flight_speed_acc, 0x1FC);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_flight_speed_min, 0x200);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_flight_speed_max, 0x204);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_hovering_acc, 0x208);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_climb_speed, 0x20C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_stun, 0x210);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_brake_acc_sand, 0x214);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_run_acc_sand, 0x218);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_jump_speed_sand, 0x21C);
    MARATHON_ASSERT_OFFSETOF(CommonContext, c_psi_throw_speed, 0x220);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_Field224, 0x224);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_Field228, 0x228);
    MARATHON_ASSERT_OFFSETOF(CommonContext, m_Field22C, 0x22C);
    MARATHON_ASSERT_SIZEOF(CommonContext, 0x230);
}
