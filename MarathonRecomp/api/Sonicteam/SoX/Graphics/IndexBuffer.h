#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Texture.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Graphics
{
    class IndexBuffer : public SoX::RefCountObject
    {
    public:
        be<uint32_t> m_Index;
    };

    MARATHON_ASSERT_OFFSETOF(IndexBuffer, m_Index, 0x8);
    MARATHON_ASSERT_SIZEOF(IndexBuffer, 0xC);
}
