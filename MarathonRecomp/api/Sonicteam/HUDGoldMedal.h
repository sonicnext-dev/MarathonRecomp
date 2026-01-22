#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/CsdObject.h>
#include <Sonicteam/HudTextParts.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextEntity.h>

namespace Sonicteam
{
    class HUDGoldMedal : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        xpointer<CsdObject> m_pCsdObject;
        xpointer<HudTextParts> m_pMedalCountText;
        xpointer<SoX::Engine::Doc> m_pDoc;
        MARATHON_INSERT_PADDING(0x3C);
        be<uint32_t> m_EpisodeIndex;
        be<uint32_t> m_ScrollIndex;
        be<uint32_t> m_MedalCount;
        boost::shared_ptr<TextCard> m_spMedalCountTextCard;
        MARATHON_INSERT_PADDING(0x10);
        boost::shared_ptr<TextCard> m_aspTextCards[5];
        boost::shared_ptr<TextEntity> m_aspTextEntities[5];
    };

    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_pCsdObject, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_pMedalCountText, 0x58);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_pDoc, 0x5C);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_EpisodeIndex, 0x9C);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_ScrollIndex, 0xA0);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_MedalCount, 0xA4);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_spMedalCountTextCard, 0xA8);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_aspTextCards, 0xC0);
    MARATHON_ASSERT_OFFSETOF(HUDGoldMedal, m_aspTextEntities, 0xE8);
    MARATHON_ASSERT_SIZEOF(HUDGoldMedal, 0x110);
}
