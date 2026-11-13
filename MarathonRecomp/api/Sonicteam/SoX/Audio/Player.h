#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Audio/SoundBank.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <stdx/string.h>

namespace Sonicteam::SoX::Audio
{
    class Cue;

    class Player
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x14);
            be<uint32_t> fpGetSoundBankID;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpGetSoundID;
            MARATHON_INSERT_PADDING(0x10);
            be<uint32_t> fpCreateCue;
        };

        xpointer<Vftable> m_pVftable;

        int GetSoundBankID(const char* in_pName)
        {
            auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<int>(m_pVftable->fpGetSoundBankID, this, name->c_str());
        }

        int GetSoundID(int in_soundBankId, const char* in_pName)
        {
            auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<int>(m_pVftable->fpGetSoundID, this, in_soundBankId, name->c_str());
        }

        SoX::RefSharedPointer<Cue> CreateCue(int in_soundId)
        {
            auto cue = guest_stack_var<SoX::RefSharedPointer<Cue>>();

            GuestToHostFunction<void*>(m_pVftable->fpCreateCue, cue.get(), this, in_soundId);

            return *cue;
        }
    };

    MARATHON_ASSERT_OFFSETOF(Player, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Player, 0x04);
}
