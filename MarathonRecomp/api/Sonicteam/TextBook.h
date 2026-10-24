#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/TextBookCategory.h>
#include <Sonicteam/TextBookMgr.h>
#include <Sonicteam/TextBoard.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextFont.h>
#include <stdx/compare/string_compare.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class TextBook : public SoX::IResource2<TextBook, TextBookMgr>
    {
    public:
        struct TextBookContext
        {
            boost::shared_ptr<TextFont> TextFonts[3];
            boost::shared_ptr<TextBoard> TextBoards[3];
            stdx::map<stdx::string, boost::shared_ptr<TextBookCategory>, stdx::string_compare> Categories;
        };

        xpointer<TextBookContext> m_pContext;
        boost::shared_ptr<uint8_t> m_spResource;
        stdx::string m_Name;
        stdx::map<stdx::string, boost::shared_ptr<TextCard>, stdx::string_compare> m_TextCards;
    };

    MARATHON_ASSERT_OFFSETOF(TextBook::TextBookContext, TextFonts, 0x00);
    MARATHON_ASSERT_OFFSETOF(TextBook::TextBookContext, TextBoards, 0x18);
    MARATHON_ASSERT_OFFSETOF(TextBook::TextBookContext, Categories, 0x30);
    MARATHON_ASSERT_SIZEOF(TextBook::TextBookContext, 0x3C);

    MARATHON_ASSERT_OFFSETOF(TextBook, m_pContext, 0x64);
    MARATHON_ASSERT_OFFSETOF(TextBook, m_spResource, 0x68);
    MARATHON_ASSERT_OFFSETOF(TextBook, m_Name, 0x70);
    MARATHON_ASSERT_OFFSETOF(TextBook, m_TextCards, 0x8C);
    MARATHON_ASSERT_SIZEOF(TextBook, 0x98);
}
