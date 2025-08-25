#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class Shape : public RefCountObject 
    {
    public:
        struct Vftable: RefCountObject::Vftable
        {
            be<uint32_t> ShapeVFT4; //(_out VECTOR* u1,__out VECTOR* u2)
            be<uint32_t> InitializeVolume;
        };

        be<uint32_t> m_ShapeType;
        be<float> m_ShapeVolume; //w * h *... * FLT_MIN
    };
}
