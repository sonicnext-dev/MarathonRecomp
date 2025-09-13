#pragma once

#include <Marathon.inl>
#include <Sonicteam/TextBookMgr.h>
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
}
