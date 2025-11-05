#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Audio/Player.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Audio
{
    class Cue : public RefCountObject
    {
    public:
        struct Vftable : RefCountObject::Vftable
        {
            MARATHON_INSERT_PADDING(0x18);
            be<uint32_t> fpSetPause;
        };

        MARATHON_INSERT_PADDING(0x0C);
        LinkNode<Cue> m_Field14;
        MARATHON_INSERT_PADDING(8);
        xpointer<Player> m_pPlayer;

        template <typename T = Player>
        T* GetPlayer()
        {
            return (T*)m_pPlayer.get();
        }

        void SetPause(bool isPaused)
        {
            GuestToHostFunction<int>(((Vftable*)m_pVftable.get())->fpSetPause, this, isPaused);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Cue, m_Field14, 0x14);
    MARATHON_ASSERT_OFFSETOF(Cue, m_pPlayer, 0x28);
    MARATHON_ASSERT_SIZEOF(Cue, 0x2C);
}
