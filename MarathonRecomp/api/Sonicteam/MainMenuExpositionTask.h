#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextEntity.h>

namespace Sonicteam
{
    class MainMenuExpositionTask : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        enum MainMenuExpositionState : uint32_t
        {
            MainMenuExpositionState_StartFade = 1,
            MainMenuExpositionState_EndFade,
            MainMenuExpositionState_3,
            MainMenuExpositionState_4,
        };

        be<MainMenuExpositionState> m_State;
        be<float> m_FadeTime;
        be<float> m_FadeSpeed;
        boost::shared_ptr<TextEntity> m_spDescriptionEntity;
        boost::shared_ptr<TextEntity> m_spPrevDescriptionEntity;
        boost::shared_ptr<TextCard> m_spDescriptionCard;
    };

    MARATHON_ASSERT_OFFSETOF(MainMenuExpositionTask, m_State, 0x54);
    MARATHON_ASSERT_OFFSETOF(MainMenuExpositionTask, m_FadeTime, 0x58);
    MARATHON_ASSERT_OFFSETOF(MainMenuExpositionTask, m_FadeSpeed, 0x5C);
    MARATHON_ASSERT_OFFSETOF(MainMenuExpositionTask, m_spDescriptionEntity, 0x60);
    MARATHON_ASSERT_OFFSETOF(MainMenuExpositionTask, m_spPrevDescriptionEntity, 0x68);
    MARATHON_ASSERT_OFFSETOF(MainMenuExpositionTask, m_spDescriptionCard, 0x70);
    MARATHON_ASSERT_SIZEOF(MainMenuExpositionTask, 0x78);
}
