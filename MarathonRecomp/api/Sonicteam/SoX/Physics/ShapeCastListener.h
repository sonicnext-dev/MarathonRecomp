#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class ShapeCast;

    class ShapeCastListener : public RefCountObject
    {
    public:
        struct Vftable : public RefCountObject::Vftable
        {
            be<uint32_t> fpCallback;
            be<uint32_t> fpClone;
        };

        void Callback(Math::Vector* in_pVector)
        {
            GuestToHostFunction<void>(static_cast<Vftable*>(m_pVftable.get())->fpCallback.get(), this, in_pVector);
        }

        bool Clone(ShapeCast* in_pOther)
        {
            return GuestToHostFunction<int>(static_cast<Vftable*>(m_pVftable.get())->fpClone.get(), this, in_pOther);
        }
    };

    MARATHON_ASSERT_SIZEOF(ShapeCastListener, 0x08);
}
