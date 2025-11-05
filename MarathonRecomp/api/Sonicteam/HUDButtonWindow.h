#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Chao/CSD/Manager/csdmSceneObserver.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/CsdResource.h>
#include <Sonicteam/HudTextParts.h>
#include <Sonicteam/TextBook.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextEntity.h>

namespace Sonicteam
{
    class HUDButtonWindow;

    class HUDButtonWindowNextObserver : public SoX::RefCountObject, public Chao::CSD::CSceneObserver
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<HUDButtonWindow> m_pHUDButtonWindow;
        MARATHON_INSERT_PADDING(4);
    };

    class HUDButtonWindow : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        xpointer<TextBook> m_pTextBook;
        xpointer<CsdResource> m_pCsdResource;
        HUDButtonWindowNextObserver m_NextObserver;
        be<float> m_Field80;
        boost::shared_ptr<TextCard> m_spTextCard;
        boost::shared_ptr<TextEntity> m_spTextEntity;
        xpointer<HudTextParts> m_pHudTextParts;
        be<float> m_Field98;
        bool m_IsIntroAnimStarted;
    };

    MARATHON_ASSERT_OFFSETOF(HUDButtonWindowNextObserver, m_pHUDButtonWindow, 0x1C);
    MARATHON_ASSERT_SIZEOF(HUDButtonWindowNextObserver, 0x24);

    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_pTextBook, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_pCsdResource, 0x58);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_NextObserver, 0x5C);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_Field80, 0x80);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_spTextCard, 0x84);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_spTextEntity, 0x8C);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_pHudTextParts, 0x94);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_Field98, 0x98);
    MARATHON_ASSERT_OFFSETOF(HUDButtonWindow, m_IsIntroAnimStarted, 0x9C);
    MARATHON_ASSERT_SIZEOF(HUDButtonWindow, 0xA0);
}
