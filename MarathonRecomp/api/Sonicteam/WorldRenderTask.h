#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/GTask.h>

namespace Sonicteam
{
    class WorldRenderTask : public SoX::Engine::GTask
    {
    public:
        be<uint32_t> m_PassIndex;
        xpointer<void> m_Camera;
        bool m_Sectors;
        bool m_Drawables;
        be<uint32_t> m_Flags;
    };

    MARATHON_ASSERT_SIZEOF(WorldRenderTask, 0x44);
}
