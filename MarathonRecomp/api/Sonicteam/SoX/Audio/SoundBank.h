#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Audio/SoundBankMgr.h>
#include <Sonicteam/SoX/IResource2.h>

namespace Sonicteam::SoX::Audio
{
    class SoundBank : public IResource2<SoundBank, SoundBankMgr>
    {
    public:
        struct Vftable : public IResource2<SoundBank, SoundBankMgr>::Vftable
        {
            MARATHON_INSERT_PADDING(0x08);
            be<uint32_t> fpGetSound;
            MARATHON_INSERT_PADDING(0x08);
        };

        MARATHON_INSERT_PADDING(0x14);
    };

    MARATHON_ASSERT_SIZEOF(SoundBank, 0x78);
}
