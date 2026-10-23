#pragma once

#include <Marathon.h>
#include <hid/message/messages.h>
#include <hid/hid.h>
#include <patches/hook_event.h>

namespace HID
{
    class LoadingLEDEvent : public ContextHookEvent<Sonicteam::LoadingTask>
    {
    public:
        virtual void Prefix(Sonicteam::LoadingTask* in_pThis) override
        {
            auto character = "sonic";

            // hell
            if (const auto pStageTitleTask = in_pThis->Find<Sonicteam::StageTitleTask>("StageTitleTask"))
            {
                if (const auto pHUDStageTitle = pStageTitleTask->Find<Sonicteam::HUDStageTitle>("HUDStageTitle"))
                {
                    if (const auto& rCsdOverlay = pHUDStageTitle->m_pCsdOverlay)
                    {
                        const auto& rResourceName = rCsdOverlay->m_pCsdResource->m_MgrResourceName;

                        for (const auto& [rSceneKey, rSceneValue] : rCsdOverlay->m_pCsdProject->m_Scenes)
                        {
                            for (const auto& [rNodeKey, rNodeValue] : rSceneValue->m_pObject->m_ActiveNodes)
                            {
                                const auto patternIndex = rNodeValue->m_pObject->m_pMotionPattern->GetPatternIndex();

                                if (rResourceName == "sprite/interlude/tips_x/tips_so_x")
                                {
                                    if (patternIndex == 3)
                                        character = "tails";
                                }
                                else if (rResourceName == "sprite/interlude/tips_x/tips_sh_x")
                                {
                                    if (patternIndex == 3)
                                    {
                                        character = "rouge";
                                    }
                                    else
                                    {
                                        character = "shadow";
                                    }
                                }
                                else if (rResourceName == "sprite/interlude/tips_x/tips_si_x")
                                {
                                    if (patternIndex == 3)
                                    {
                                        character = "blaze";
                                    }
                                    else
                                    {
                                        character = "silver";
                                    }
                                }
                            }
                        }
                    }
                }
            }

            HID::GetManager()->BroadcastMessage<HID::MsgSetCharacterLED>(character);
        }
    };
}
