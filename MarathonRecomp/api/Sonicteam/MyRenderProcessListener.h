#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/RenderProcessListener.h>

namespace Sonicteam
{
    class MyRenderProcessListener : public SoX::Engine::RenderProcessListener {};

    MARATHON_ASSERT_SIZEOF(MyRenderProcessListener, 0x10);
}
