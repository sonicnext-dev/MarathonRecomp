#pragma once

#include <Marathon.inl>
#include <Sonicteam/TextFontPictureMgr.h>

namespace Sonicteam
{
    class TextFontPicture : public SoX::IResource2<TextFontPicture, TextFontPictureMgr>
    {
    public:
        xpointer<uint8_t> m_pResource;
        xpointer<MyTexture> m_pTexture;
        be<float> m_TextureWidth;
        be<float> m_TextureHeight;
        be<float> m_Field74;
        xpointer<void> m_Field78;
        be<uint32_t> m_CropCount;
    };

    MARATHON_ASSERT_OFFSETOF(TextFontPicture, m_pResource, 0x64);
    MARATHON_ASSERT_OFFSETOF(TextFontPicture, m_pTexture, 0x68);
    MARATHON_ASSERT_OFFSETOF(TextFontPicture, m_TextureWidth, 0x6C);
    MARATHON_ASSERT_OFFSETOF(TextFontPicture, m_TextureHeight, 0x70);
    MARATHON_ASSERT_OFFSETOF(TextFontPicture, m_Field74, 0x74);
    MARATHON_ASSERT_OFFSETOF(TextFontPicture, m_Field78, 0x78);
    MARATHON_ASSERT_OFFSETOF(TextFontPicture, m_CropCount, 0x7C);
}
