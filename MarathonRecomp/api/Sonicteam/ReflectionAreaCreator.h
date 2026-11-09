#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam
{
    class ReflectionAreaCreator : public SoX::RefCountObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDtor;
            be<uint32_t> fpCreate;
        };
    };
}
