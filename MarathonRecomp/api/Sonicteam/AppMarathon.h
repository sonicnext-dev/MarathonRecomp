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
        xpointer<DocMarathonState> m_pDoc;

        static AppMarathon* GetInstance();

        GameImp* GetGame() const
        {
            if (auto pGameMode = m_pDoc->GetDocMode<Sonicteam::GameMode>())
                return pGameMode->GetGame();

            return nullptr;
        }
    };
}

#include <Sonicteam/AppMarathon.inl>
