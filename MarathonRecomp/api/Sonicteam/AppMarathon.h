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

        AppMarathon* GetInstance()
        {
            return *(xpointer<AppMarathon>*)MmGetHostAddress(0x82D3B348);
        }

        GameImp* GetGame() const
        {
            if (auto pGameMode = m_pDoc->GetDocMode<Sonicteam::GameMode>())
                return pGameMode->GetGame();

            return nullptr;
        }
    };

    MARATHON_ASSERT_OFFSETOF(AppMarathon, m_pDoc, 0x180);
}
