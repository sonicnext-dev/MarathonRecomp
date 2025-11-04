#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>

namespace Sonicteam::SoX::Graphics
{
    class Shader : public IResource
    {
    public:
        MARATHON_INSERT_PADDING(0xC);
    };

    MARATHON_ASSERT_SIZEOF(Shader, 0x70);
}
