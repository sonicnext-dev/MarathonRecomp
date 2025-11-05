#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/TextBookMgr.h>
#include <Sonicteam/TextCard.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class TextBook : public SoX::IResource2<TextBook, TextBookMgr>
    {
    public:
        struct Entry
        {
            xpointer<Entry> Field00;
            xpointer<Entry> Previous;
            xpointer<Entry> Next;
            stdx::string Name;
            boost::shared_ptr<TextCard> spTextCard;
            MARATHON_INSERT_PADDING(1);
            bool Field31;
            MARATHON_INSERT_PADDING(2);
        };

        MARATHON_INSERT_PADDING(4);
        boost::shared_ptr<uint8_t> m_spResource;
        stdx::string m_Name;
        MARATHON_INSERT_PADDING(4);
        xpointer<Entry> m_pRootEntry;
        be<uint32_t> m_EntryCount;

        const char* FindName(const uint16_t* pText) const
        {
            auto pCardTable = m_spResource.get() + 0x2C;

            for (size_t i = 0; i < m_EntryCount; i++)
            {
                auto pCard = pCardTable + (i * 0x0C);
                auto pCardName = *(xpointer<const char>*)pCard;
                auto pCardText = *(xpointer<const uint16_t>*)(pCard + 4);

                if (!strcmpU16(pCardText, pText))
                    continue;

                return pCardName;
            }

            return nullptr;
        }
    };

    MARATHON_ASSERT_OFFSETOF(TextBook::Entry, Field00, 0x00);
    MARATHON_ASSERT_OFFSETOF(TextBook::Entry, Previous, 0x04);
    MARATHON_ASSERT_OFFSETOF(TextBook::Entry, Next, 0x08);
    MARATHON_ASSERT_OFFSETOF(TextBook::Entry, Name, 0x0C);
    MARATHON_ASSERT_OFFSETOF(TextBook::Entry, spTextCard, 0x28);
    MARATHON_ASSERT_OFFSETOF(TextBook::Entry, Field31, 0x31);
    MARATHON_ASSERT_SIZEOF(TextBook::Entry, 0x34);

    MARATHON_ASSERT_OFFSETOF(TextBook, m_spResource, 0x68);
    MARATHON_ASSERT_OFFSETOF(TextBook, m_Name, 0x70);
    MARATHON_ASSERT_OFFSETOF(TextBook, m_pRootEntry, 0x90);
    MARATHON_ASSERT_OFFSETOF(TextBook, m_EntryCount, 0x94);
    MARATHON_ASSERT_SIZEOF(TextBook, 0x98);
}
