#pragma once

#include <Marathon.inl>
#include <Sonicteam/CsdLink.h>
#include <stdx/wstring.h>

namespace Sonicteam
{
    class TextFont;
    class TextBoard;

    class TextEntity : public CsdLink
    {
    public:
        struct Vertex
        {
            be<float> X;
            be<float> Y;
            MARATHON_INSERT_PADDING(0x10);
            be<uint32_t> Colour; // ARGB8888
            be<float> U;
            be<float> V;
            MARATHON_INSERT_PADDING(0x18);
        };

        MARATHON_INSERT_PADDING(4);
        boost::shared_ptr<TextFont> m_spTextFont;
        boost::shared_ptr<TextBoard> m_spTextBoard;
        MARATHON_INSERT_PADDING(4);
        be<float> m_X;
        be<float> m_Y;
        MARATHON_INSERT_PADDING(8);
        stdx::wstring m_Text;
        stdx::wstring m_Field5C;
        MARATHON_INSERT_PADDING(4);
        xpointer<const char> m_pVariables;
        stdx::wstring m_Field80;
        MARATHON_INSERT_PADDING(0x14);
        be<float> m_Width;
        be<float> m_FieldB4;
        be<float> m_FieldB8;
        be<float> m_FieldBC;
        MARATHON_INSERT_PADDING(0x0C);
        be<float> m_ScaleX;
        be<float> m_ScaleY;
        MARATHON_INSERT_PADDING(8);
        bool m_FieldDC;
        bool m_FieldDD;
        MARATHON_INSERT_PADDING(2);
        xpointer<void> m_FieldE0;              // Only present when there's character vertices.
        xpointer<void> m_FieldE4;              // Only present when there's image vertices.
        xpointer<Vertex> m_pCharacterVertices; // BL/TL/TR BL/TR/BR (two triangles per character)
        xpointer<Vertex> m_pImageVertices;     // BL/TL/TR BL/TR/BR (two triangles per image)
        be<uint32_t> m_CharacterVertexCount;
        be<uint32_t> m_ImageVertexCount;
        be<uint32_t> m_TextLength;
        MARATHON_INSERT_PADDING(4);
        be<float> m_Field100;
        be<float> m_Field104;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Width, 0xB0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldE0, 0xE0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldE4, 0xE4);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_pCharacterVertices, 0xE8);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_pImageVertices, 0xEC);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_CharacterVertexCount, 0xF0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_ImageVertexCount, 0xF4);
}
