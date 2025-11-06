#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/ApplicationXenon.h>
#include <Sonicteam/DocMarathonState.h>
#include <Sonicteam/GameMode.h>

namespace Sonicteam
{
    class AppMarathon : public SoX::ApplicationXenon
    {
    public:
        xpointer<DocMarathonState> m_pDocState;

        static AppMarathon* GetInstance();

        GameImp* GetGame() const
        {
            if (auto pGameMode = m_pDocState->GetDocMode<GameMode>())
                return pGameMode->GetGame();

            return nullptr;
        }
    };

    MARATHON_ASSERT_OFFSETOF(AppMarathon, m_pDocState, 0x180);
}

#include <Sonicteam/AppMarathon.inl>
