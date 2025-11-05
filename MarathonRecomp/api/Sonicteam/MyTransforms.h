#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Transforms.h>

namespace Sonicteam
{
    class MyTransforms : public SoX::Graphics::Transforms
    {
    public:
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_SIZEOF(MyTransforms, 0xF8);
}
