#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/ShapeType.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class Shape : public RefCountObject 
    {
    public:
        struct Vftable : public RefCountObject::Vftable
        {
            be<uint32_t> fpGetAABB;
            be<uint32_t> fpInit;
        };

        be<ShapeType> m_Type;
        be<float> m_Volume;

        void GetAABB(Math::Vector* out_pMin, Math::Vector* out_pMax)
        {
            GuestToHostFunction<void>(static_cast<Vftable*>(m_pVftable.get())->fpGetAABB.get(), this, out_pMin, out_pMax);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Shape, m_Type, 0x08);
    MARATHON_ASSERT_OFFSETOF(Shape, m_Volume, 0x0C);
    MARATHON_ASSERT_SIZEOF(Shape, 0x10);
}
