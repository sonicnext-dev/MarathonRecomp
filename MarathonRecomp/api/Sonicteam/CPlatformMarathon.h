#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdPlatform.h>
#include <Sonicteam/SoX/Graphics/DrawPrimitiveCache.h>
#include <Sonicteam/CMemoryAllocMarathon.h>
#include <Sonicteam/CReadFileMarathon.h>
#include <Sonicteam/CTexListMarathon.h>
#include <Sonicteam/MyGraphicsDevice.h>

namespace Sonicteam
{
    class CPlatformMarathon : public Chao::CSD::CPlatform
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<MyGraphicsDevice> m_pDevice;
        xpointer<SoX::Graphics::DrawPrimitiveCache> m_pDrawPrimitiveCache;
        MARATHON_INSERT_PADDING(0x08);
        CMemoryAllocMarathon m_MemoryAlloc;
        CReadFileMarathon m_ReadFile;
        CTexListMarathon m_TexList;
    };

    MARATHON_ASSERT_OFFSETOF(CPlatformMarathon, m_pDevice, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CPlatformMarathon, m_pDrawPrimitiveCache, 0x10);
    MARATHON_ASSERT_OFFSETOF(CPlatformMarathon, m_MemoryAlloc, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CPlatformMarathon, m_ReadFile, 0x20);
    MARATHON_ASSERT_OFFSETOF(CPlatformMarathon, m_TexList, 0x40);
    MARATHON_ASSERT_SIZEOF(CPlatformMarathon, 0x60);
}
