#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/MyTexture.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class MovieObjectWmv : public SoX::RefCountObject
    {
    public:
        bool m_UseCustomDimensions;
        be<float> m_Left;
        be<float> m_Top;
        be<float> m_Right;
        be<float> m_Bottom;
        MARATHON_INSERT_PADDING(4);
        stdx::string m_FilePath;
        MARATHON_INSERT_PADDING(8);
        bool m_RenderMovie;
        MARATHON_INSERT_PADDING(0x38);
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        be<uint32_t> m_Field84;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<MyTexture> m_apTexturesYUV[4 * 3];
        MARATHON_INSERT_PADDING(8);
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<void> m_pTechnique;
        stdx::string m_Language;
    };

    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_UseCustomDimensions, 0x08);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Left, 0x0C);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Top, 0x10);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Right, 0x14);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Bottom, 0x18);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_FilePath, 0x20);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_RenderMovie, 0x44);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_pMyGraphicsDevice, 0x80);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Field84, 0x84);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_apTexturesYUV, 0x94);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Width, 0xCC);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Height, 0xD0);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_pTechnique, 0xE0);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Language, 0xE4);
    MARATHON_ASSERT_SIZEOF(MovieObjectWmv, 0x100);
}
