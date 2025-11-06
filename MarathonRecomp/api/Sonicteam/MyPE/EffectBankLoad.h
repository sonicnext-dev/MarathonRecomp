#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/MyPE/MyEffectBank.h>

namespace Sonicteam::MyPE
{
    class EffectBankLoadManager;

    class TextureBankLoad; // Implement later

    class EffectBankLoad : public SoX::IResource2<EffectBankLoad, EffectBankLoadManager>
    {
    public:
        SoX::RefSharedPointer<MyEffectBank> m_spMyEffectBank;
    };
}
