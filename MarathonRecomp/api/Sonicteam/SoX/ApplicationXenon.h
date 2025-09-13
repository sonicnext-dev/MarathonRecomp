#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Application.h>

namespace Sonicteam::SoX
{
    class ApplicationXenon : public Engine::Application
    {
    public:
        MARATHON_INSERT_PADDING(0x174);

        static ApplicationXenon* GetInstance();
    };
}

#include <Sonicteam/SoX/ApplicationXenon.inl>
