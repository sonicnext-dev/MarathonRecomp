#pragma once

#include <Marathon.inl>
#include <Sonicteam/CsdLink.h>

namespace Sonicteam
{
    class TextFont;
    class TextBoard;

    struct TextEntityVertex
    {
        be<float> X;
        be<float> Y;
        be<float> Field08;
        be<float> Field0C;
        be<float> Field10;
        be<float> Field14;
        be<uint32_t> Colour; // ARGB8888
        be<float> U;
        be<float> V;
        be<float> Field24;
        be<float> Field28;
        be<float> Field2C;
        be<float> Field30;
        be<float> Field34;
        be<float> Field38;
    };

    class TextEntity : public CsdLink
    {
    public:
        MARATHON_INSERT_PADDING(4);
        boost::shared_ptr<TextFont> m_spTextFont;
        boost::shared_ptr<TextBoard> m_spTextBoard;
        MARATHON_INSERT_PADDING(4);
        be<float> m_X;
        be<float> m_Y;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<const wchar_t> m_pText;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_TextLength;
        MARATHON_INSERT_PADDING(0x24);
        xpointer<const char> m_pVariables;
        MARATHON_INSERT_PADDING(0x4C);
        be<float> m_ScaleX;
        be<float> m_ScaleY;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<void> m_FieldE0;                        // Only present when there's character vertices.
        xpointer<void> m_FieldE4;                        // Only present when there's image vertices.
        xpointer<TextEntityVertex> m_pCharacterVertices; // BL/TL/TR BL/TR/BR (two triangles per character)
        xpointer<TextEntityVertex> m_pImageVertices;     // BL/TL/TR BL/TR/BR (two triangles per image)
        be<uint32_t> m_CharacterVertexCount;
        be<uint32_t> m_ImageVertexCount;

        std::pair<float, float> Measure() const
        {
            float w{};
            float h{};

            if (m_TextLength <= 0)
                return { w, h };

            if (m_FieldE0)
            {
                for (int i = 0; i < m_CharacterVertexCount; i += 6)
                {
                    auto& tl = m_pCharacterVertices[i + 0];
                    auto& bl = m_pCharacterVertices[i + 1];
                    auto& tr = m_pCharacterVertices[i + 2];

                    w += tr.X - tl.X;
                    h = std::max(h, bl.Y - tl.Y);
                }
            }

            if (m_FieldE4)
            {
                for (int i = 0; i < m_ImageVertexCount; i += 6)
                {
                    auto& tl = m_pImageVertices[i + 0];
                    auto& bl = m_pImageVertices[i + 1];
                    auto& tr = m_pImageVertices[i + 2];

                    w += tr.X - tl.X;
                    h = std::max(h, bl.Y - tl.Y);
                }
            }

            return { w, h };
        }
    };

    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldE0, 0xE0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldE4, 0xE4);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_pCharacterVertices, 0xE8);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_pImageVertices, 0xEC);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_CharacterVertexCount, 0xF0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_ImageVertexCount, 0xF4);
}
