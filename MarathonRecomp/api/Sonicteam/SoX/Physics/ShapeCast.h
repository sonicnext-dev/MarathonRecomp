#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Physics/Shape.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::SoX::Physics
{
    class ShapeCast : public SoX::RefCountObject
    {
    public:
        struct Vftable:SoX::RefCountObject::Vftable
        {
            be<uint32_t> m_fpFunc4;
            be<uint32_t> m_fpFunc8;
            be<uint32_t> m_fpFuncC;
            be<uint32_t> m_fpFunc10;
            be<uint32_t> m_fpFunc14;
            be<uint32_t> m_fpFunc18;
            be<uint32_t> m_fpFunc1C;
            be<uint32_t> m_fpFunc20;
            be<uint32_t> m_fpFunc24;
            be<uint32_t> m_fpFunc28;
            be<uint32_t> m_fpFunc2C;
            be<uint32_t> m_fpFunc30;
            be<uint32_t> m_fpSetShape;
            be<uint32_t> m_fpFunc38;
            be<uint32_t> m_fpFunc3C;
        };
        xpointer<SoX::Physics::Shape> m_Shape;

        void Func38(SoX::Math::Vector* points, SoX::Physics::ShapeCastListener* listener)
        {
            Vftable* vft = (Vftable*)this->m_pVftable.get();
            GuestToHostFunction<void>(vft->m_fpFunc38.get(),this,points, listener);
        }

        void Func3C(SoX::Math::Vector* points, SoX::Physics::ShapeCastListener* listener)
        {
            Vftable* vft = (Vftable*)this->m_pVftable.get();
            GuestToHostFunction<void>(vft->m_fpFunc3C.get(),this,points, listener);
        }
    };

    MARATHON_ASSERT_SIZEOF(ShapeCast, 0xC);
}
