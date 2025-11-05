#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class Shape : public RefCountObject 
    {
    public:
        struct Vftable : public RefCountObject::Vftable
        {
            be<uint32_t> fpVFunction04; // (__out VECTOR* u1, __out VECTOR* u2)
            be<uint32_t> fpInitializeVolume;
        };

        be<uint32_t> m_ShapeType;
        be<float> m_ShapeVolume; // w * h * ... * FLT_MIN
    };

    MARATHON_ASSERT_OFFSETOF(Shape, m_ShapeType, 0x08);
    MARATHON_ASSERT_OFFSETOF(Shape, m_ShapeVolume, 0x0C);
}
