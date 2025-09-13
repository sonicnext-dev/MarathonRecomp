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
}
