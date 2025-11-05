#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDMainDisplay : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x24);
        be<Character> m_Character;
        MARATHON_INSERT_PADDING(0x108);
        boost::shared_ptr<TextEntity> m_Field184;
        boost::shared_ptr<TextEntity> m_spTrickPointText;
        boost::shared_ptr<TextEntity> m_Field194;
        MARATHON_INSERT_PADDING(0x10);
        boost::shared_ptr<TextEntity> m_spSavePointTimeText;
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_OFFSETOF(HUDMainDisplay, m_Character, 0x78);
    MARATHON_ASSERT_OFFSETOF(HUDMainDisplay, m_Field184, 0x184);
    MARATHON_ASSERT_OFFSETOF(HUDMainDisplay, m_spTrickPointText, 0x18C);
    MARATHON_ASSERT_OFFSETOF(HUDMainDisplay, m_Field194, 0x194);
    MARATHON_ASSERT_OFFSETOF(HUDMainDisplay, m_spSavePointTimeText, 0x1AC);
    MARATHON_ASSERT_SIZEOF(HUDMainDisplay, 0x1E0);
}
