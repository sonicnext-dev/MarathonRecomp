#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/CsdObject.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextEntity.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class HudTextParts : public SoX::RefCountObject
    {
    public:
        xpointer<HudTextParts> m_pNext;
        xpointer<DocMarathonState> m_pDoc;
        boost::shared_ptr<TextCard> m_spTextCard;
        boost::shared_ptr<TextEntity> m_spTextEntity;
        xpointer<CsdObject> m_pCsdObject;
        stdx::string m_SceneName;
        stdx::string m_CastName;
        be<float> m_Priority;
        be<float> m_OffsetX;
        be<float> m_OffsetY;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_AlignmentFlags;

        HudTextParts* Find(const char* pSceneName)
        {
            auto pRoot = this;

            while (pRoot)
            {
                if (pRoot->m_SceneName == pSceneName)
                    return pRoot;

                pRoot = pRoot->m_pNext;
            }

            return nullptr;
        }

        HudTextParts* Find(const char* pSceneName, const char* pCastName)
        {
            auto pRoot = this;

            while (pRoot)
            {
                if (pRoot->m_SceneName == pSceneName && pRoot->m_CastName == pCastName)
                    return pRoot;

                pRoot = pRoot->m_pNext;
            }

            return nullptr;
        }
    };

    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_pNext, 0x08);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_pDoc, 0x0C);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_spTextCard, 0x10);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_spTextEntity, 0x18);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_pCsdObject, 0x20);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_SceneName, 0x24);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_CastName, 0x40);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_Priority, 0x5C);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_OffsetX, 0x60);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_OffsetY, 0x64);
    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_AlignmentFlags, 0x6C);
    MARATHON_ASSERT_SIZEOF(HudTextParts, 0x70);
}
