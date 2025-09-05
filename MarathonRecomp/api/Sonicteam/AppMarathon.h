#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/ApplicationXenon.h>
#include <Sonicteam/DocMarathonState.h>

namespace Sonicteam
{
    class AppMarathon : public SoX::ApplicationXenon
    {
    public:
        xpointer<DocMarathonState> m_pDoc;

        static AppMarathon* GetInstance();
    };
}

#include <Sonicteam/AppMarathon.inl>
