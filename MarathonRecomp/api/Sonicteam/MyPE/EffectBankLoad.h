#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/MyPE/MyEffectBank.h>

namespace Sonicteam::MyPE
{
    class EffectBankLoadManager;
    class TextureBankLoad;

    class EffectBankLoad : public SoX::IResource2<EffectBankLoad, EffectBankLoadManager>
    {
    public:
        SoX::RefSharedPointer<MyEffectBank> m_spMyEffectBank;
    };

    MARATHON_ASSERT_OFFSETOF(EffectBankLoad, m_spMyEffectBank, 0x64);
}
