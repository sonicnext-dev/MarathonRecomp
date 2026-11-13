#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam
{
    class SoundBankData : public SoX::RefCountObject
    {
    public:
        struct Vftable : public SoX::RefCountObject::Vftable
        {
            be<uint32_t> fpGetSoundID;
            be<uint32_t> fpGetName;
            be<uint32_t> fpGetSound;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpGetPath;
        };

        enum StreamType
        {
            StreamType_Undefined = -1,
            StreamType_CueSheet,
            StreamType_External
        };

        struct Sound
        {
            char Name[32];
            be<StreamType> StreamType;
            be<uint32_t> ID;
            be<uint32_t> Field28;
            be<float> Field2C;
            be<float> Radius;
        };

        xpointer<const char> m_pName;
        xpointer<Sound> m_pSounds;
        MARATHON_INSERT_PADDING(0x04);
        xpointer<xpointer<const char>> m_pPaths;

        int GetSoundID(const char* in_pName)
        {
            auto name = guest_stack_var<stdx::string>(in_pName);

            return GuestToHostFunction<int>(static_cast<Vftable*>(m_pVftable.get())->fpGetSoundID, this, name->c_str());
        }

        const char* GetName()
        {
            return GuestToHostFunction<const char*>(static_cast<Vftable*>(m_pVftable.get())->fpGetName, this);
        }

        Sound* GetSound(int in_index)
        {
            return GuestToHostFunction<Sound*>(static_cast<Vftable*>(m_pVftable.get())->fpGetSound, this, in_index);
        }

        const char* GetPath(int in_index)
        {
            return GuestToHostFunction<const char*>(static_cast<Vftable*>(m_pVftable.get())->fpGetPath, this, in_index);
        }
    };

    MARATHON_ASSERT_OFFSETOF(SoundBankData::Sound, Name, 0x00);
    MARATHON_ASSERT_OFFSETOF(SoundBankData::Sound, StreamType, 0x20);
    MARATHON_ASSERT_OFFSETOF(SoundBankData::Sound, ID, 0x24);
    MARATHON_ASSERT_OFFSETOF(SoundBankData::Sound, Field28, 0x28);
    MARATHON_ASSERT_OFFSETOF(SoundBankData::Sound, Field2C, 0x2C);
    MARATHON_ASSERT_OFFSETOF(SoundBankData::Sound, Radius, 0x30);
    MARATHON_ASSERT_SIZEOF(SoundBankData::Sound, 0x34);

    MARATHON_ASSERT_OFFSETOF(SoundBankData, m_pName, 0x08);
    MARATHON_ASSERT_OFFSETOF(SoundBankData, m_pSounds, 0x0C);
    MARATHON_ASSERT_OFFSETOF(SoundBankData, m_pPaths, 0x14);
    MARATHON_ASSERT_SIZEOF(SoundBankData, 0x18);
}
