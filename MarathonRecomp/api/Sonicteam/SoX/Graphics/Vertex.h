#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::SoX::Graphics
{
    struct Vertex
    {
        Math::Vector3 m_position;
        MARATHON_INSERT_PADDING(0xB);
        be<uint32_t> m_colour; // ARGB8888
        Math::Vector2 m_uv;
        Math::Vector2 m_unknown;
        MARATHON_INSERT_PADDING(0x10);
    };
}
