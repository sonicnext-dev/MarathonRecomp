#pragma once

#include <Marathon.h>
#include <hid/message/messages.h>
#include <hid/hid.h>
#include <patches/hook_event.h>

namespace HID
{
    class LoadingLEDEvent : public ContextHookEvent<Sonicteam::LoadingTask>
    {
        static const char* GetCharacter(Sonicteam::LoadingTask* in_pLoadingTask)
        {
            auto character = "sonic";

            const auto pStageTitleTask = in_pLoadingTask->Find<Sonicteam::StageTitleTask>("StageTitleTask");

            if (!pStageTitleTask)
                return character;

            const auto pHUDStageTitle = pStageTitleTask->Find<Sonicteam::HUDStageTitle>("HUDStageTitle");

            if (!pHUDStageTitle)
                return character;

            const auto& rCsdOverlay = pHUDStageTitle->m_pCsdOverlay;

            if (!rCsdOverlay)
                return character;

            enum
            {
                Sonic,
                Shadow,
                Silver
            };

            int resourceType{};

            const auto& rResourceName = rCsdOverlay->m_pCsdResource->m_MgrResourceName;

            if (rResourceName == "sprite/interlude/tips_x/tips_so_x")
            {
                resourceType = Sonic;
            }
            else if (rResourceName == "sprite/interlude/tips_x/tips_sh_x")
            {
                resourceType = Shadow;
            }
            else if (rResourceName == "sprite/interlude/tips_x/tips_si_x")
            {
                resourceType = Silver;
            }

            for (const auto& [rSceneKey, rSceneValue] : rCsdOverlay->m_pCsdProject->m_Scenes)
            {
                for (const auto& [rNodeKey, rNodeValue] : rSceneValue->m_pObject->m_ActiveNodes)
                {
                    const auto alternativePattern = rNodeValue->m_pObject->m_pMotionPattern->GetPatternIndex() == 3;

                    switch (resourceType)
                    {
                        case Sonic:
                            character = alternativePattern ? "tails" : "sonic";
                            break;
                        case Shadow:
                            character = alternativePattern ? "rouge" : "shadow";
                            break;
                        case Silver:
                            character = alternativePattern ? "blaze" : "silver";
                            break;
                    }
                }
            }

            return character;
        }

    public:
        void Prefix(Sonicteam::LoadingTask* in_pThis) override
        {
            GetManager()->BroadcastMessage<MsgSetCharacterLED>(GetCharacter(in_pThis));
        }
    };
}
