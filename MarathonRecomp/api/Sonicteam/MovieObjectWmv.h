#pragma once

#include <Marathon.inl>
#include <Cal/CCalXmvPlayer.h>
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
        MARATHON_INSERT_PADDING(0x04);
        stdx::string m_FilePath;
        MARATHON_INSERT_PADDING(0x08);
        bool m_RenderMovie;
        MARATHON_INSERT_PADDING(0x38);
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        xpointer<CCalXmvPlayer> m_pXmvPlayer;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<MyTexture> m_TexturesYUV[4 * 3];
        MARATHON_INSERT_PADDING(0x08);
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(0x04);
        be<float> m_FrameRate;
        be<float> m_Frame;
        xpointer<void> m_pTechnique;
        stdx::string m_Language;

        void SetVolume(float in_volume) const
        {
            m_pXmvPlayer->m_pChannel->m_pAudioRenderer->m_pSourceVoice->m_pVoice->SetVolume(in_volume);
        }
    };

    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_UseCustomDimensions, 0x08);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Left, 0x0C);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Top, 0x10);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Right, 0x14);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Bottom, 0x18);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_FilePath, 0x20);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_RenderMovie, 0x44);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_pMyGraphicsDevice, 0x80);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_pXmvPlayer, 0x84);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_TexturesYUV, 0x94);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Width, 0xCC);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Height, 0xD0);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_FrameRate, 0xD8);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Frame, 0xDC);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_pTechnique, 0xE0);
    MARATHON_ASSERT_OFFSETOF(MovieObjectWmv, m_Language, 0xE4);
    MARATHON_ASSERT_SIZEOF(MovieObjectWmv, 0x100);
}
