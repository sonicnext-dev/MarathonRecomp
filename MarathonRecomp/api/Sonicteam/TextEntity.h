#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Vertex.h>
#include <Sonicteam/CsdLink.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextFont.h>
#include <Sonicteam/TextBoard.h>
#include <stdx/wstring.h>

namespace Sonicteam
{
    class TextEntity : public CsdLink
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
        boost::shared_ptr<TextFont> m_spTextFont;
        boost::shared_ptr<TextBoard> m_spTextBoard;
        MARATHON_INSERT_PADDING(0x04);
        be<float> m_X;
        be<float> m_Y;
        MARATHON_INSERT_PADDING(0x08);
        stdx::wstring m_Text;
        stdx::wstring m_Page;
        MARATHON_INSERT_PADDING(0x04);
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
        MARATHON_INSERT_PADDING(0x08);
        bool m_FieldDC;
        bool m_FieldDD;
        MARATHON_INSERT_PADDING(0x02);
        xpointer<void> m_FieldE0;                            // Only present when there's text vertices.
        xpointer<void> m_FieldE4;                            // Only present when there's variable vertices.
        xpointer<SoX::Graphics::Vertex> m_pTextVertices;     // BL/TL/TR BL/TR/BR (two triangles per character)
        xpointer<SoX::Graphics::Vertex> m_pVariableVertices; // BL/TL/TR BL/TR/BR (two triangles per variable)
        be<uint32_t> m_TextVertexCount;
        be<uint32_t> m_VariableVertexCount;
        be<uint32_t> m_TextLength;
        MARATHON_INSERT_PADDING(0x04);
        be<float> m_Field100;
        be<float> m_Field104;
        be<uint32_t> m_PageIndex;
        be<uint32_t> m_PagesRemaining;

        int GetVariableCount(int in_pageIndex = -1) const
        {
            int result{};
            int pageIndex{};

            for (int i = 0; i < m_Text.size(); i++)
            {
                const auto c = ByteSwap(m_Text.c_str()[i]);

                if (c == u'\f')
                    pageIndex++;

                if (c != u'$')
                    continue;

                if (in_pageIndex < 0 || in_pageIndex == pageIndex)
                    result++;
            }

            return result;
        }

        int GetRelativeVariableCount(int in_pageIndex) const
        {
            int result{};
            int pageIndex{};

            for (int i = 0; i < m_Text.size(); i++)
            {
                const auto c = ByteSwap(m_Text.c_str()[i]);

                if (in_pageIndex == pageIndex)
                    break;

                if (c == u'\f')
                {
                    pageIndex++;
                    continue;
                }

                if (c != u'$')
                    continue;

                result++;
            }

            return result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(TextEntity, m_spTextFont, 0x1C);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_spTextBoard, 0x24);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_X, 0x30);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Y, 0x34);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Text, 0x40);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Page, 0x5C);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_pVariables, 0x7C);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Field80, 0x80);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Width, 0xB0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldB4, 0xB4);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldB8, 0xB8);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldBC, 0xBC);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_ScaleX, 0xCC);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_ScaleY, 0xD0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldDC, 0xDC);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldDD, 0xDD);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldE0, 0xE0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_FieldE4, 0xE4);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_pTextVertices, 0xE8);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_pVariableVertices, 0xEC);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_TextVertexCount, 0xF0);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_VariableVertexCount, 0xF4);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_TextLength, 0xF8);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Field100, 0x100);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_Field104, 0x104);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_PageIndex, 0x108);
    MARATHON_ASSERT_OFFSETOF(TextEntity, m_PagesRemaining, 0x10C);
    MARATHON_ASSERT_SIZEOF(TextEntity, 0x110);
}
