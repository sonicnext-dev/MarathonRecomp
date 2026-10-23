#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Math/Distance.h>
#include <Sonicteam/SoX/Physics/Shape.h>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>

namespace Sonicteam::SoX::Physics
{
    class ShapeCast : public RefCountObject
    {
    public:
        struct Vftable : public RefCountObject::Vftable
        {
            MARATHON_INSERT_PADDING(0x38);
            be<uint32_t> fpCastShape;
        };

        xpointer<Shape> m_pShape;

        bool CastShape(Math::Distance* in_pDistance, ShapeCastListener* in_pListener)
        {
            return GuestToHostFunction<int>(static_cast<Vftable*>(m_pVftable.get())->fpCastShape.get(), this, in_pDistance, in_pListener);
        }
    };

    MARATHON_ASSERT_OFFSETOF(ShapeCast, m_pShape, 0x08);
    MARATHON_ASSERT_SIZEOF(ShapeCast, 0x0C);
}
