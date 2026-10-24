#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Technique.h>
#include <Sonicteam/SoX/Graphics/Texture.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/TextFontPicture.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class TextFont
    {
    public:
        struct TextFontContext
        {
            be<uint32_t> ID;
            xpointer<const char> pTexturePath;
            be<uint16_t> TextureWidth;
            be<uint16_t> TextureHeight;
            xpointer<const char> pTextFontMapPath;
            xpointer<const char> pTextFontProportionPath;
            xpointer<const char> pTextFontPicturePath;
            be<uint16_t> CellWidth;
            be<uint16_t> CellHeight;
        };

        xpointer<TextFontContext> m_pContext;
        SoX::RefSharedPointer<> m_spShader;
        SoX::RefSharedPointer<SoX::Graphics::Technique> m_spTechniqueA;
        SoX::RefSharedPointer<SoX::Graphics::Technique> m_spTechniqueB;
        SoX::RefSharedPointer<SoX::Graphics::Technique> m_spTechniqueC;
        SoX::RefSharedPointer<SoX::Graphics::Texture> m_spTexture;
        MARATHON_INSERT_PADDING(0x2C);
        SoX::RefSharedPointer<> m_spTextFontMap;
        SoX::RefSharedPointer<> m_spTextFontProportion;
        SoX::RefSharedPointer<TextFontPicture> m_spTextFontPicture;
    };

    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, ID, 0x00);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, pTexturePath, 0x04);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, TextureWidth, 0x08);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, TextureHeight, 0x0A);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, pTextFontMapPath, 0x0C);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, pTextFontProportionPath, 0x10);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, pTextFontPicturePath, 0x14);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, CellWidth, 0x18);
    MARATHON_ASSERT_OFFSETOF(TextFont::TextFontContext, CellHeight, 0x1A);
    MARATHON_ASSERT_SIZEOF(TextFont::TextFontContext, 0x1C);

    MARATHON_ASSERT_OFFSETOF(TextFont, m_pContext, 0x00);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spShader, 0x04);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spTechniqueA, 0x08);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spTechniqueB, 0x0C);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spTechniqueC, 0x10);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spTexture, 0x14);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spTextFontMap, 0x44);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spTextFontProportion, 0x48);
    MARATHON_ASSERT_OFFSETOF(TextFont, m_spTextFontPicture, 0x4C);
    MARATHON_ASSERT_SIZEOF(TextFont, 0x50);
}
