#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam
{
    class ISceneParam : public SoX::RefCountObject
    {
    public:
        struct Vftable : public SoX::RefCountObject::Vftable
        {
            be<uint32_t> fpGetName;
            be<uint32_t> fpInit;
            be<uint32_t> fpUpdateSerial;
            be<uint32_t> fpUpdateParallel;
        };

        const char* GetName()
        {
            return GuestToHostFunction<const char*>(static_cast<Vftable*>(m_pVftable.get())->fpGetName.get(), this);
        }

        ISceneParam* UpdateSerial(double in_deltaTime = 0.0)
        {
            return GuestToHostFunction<ISceneParam*>(static_cast<Vftable*>(m_pVftable.get())->fpUpdateSerial.get(), this, in_deltaTime);
        }
    };

    MARATHON_ASSERT_OFFSETOF(ISceneParam::Vftable, fpGetName, 0x04);
    MARATHON_ASSERT_OFFSETOF(ISceneParam::Vftable, fpInit, 0x08);
    MARATHON_ASSERT_OFFSETOF(ISceneParam::Vftable, fpUpdateSerial, 0x0C);
    MARATHON_ASSERT_SIZEOF(ISceneParam::Vftable, 0x14);
}
