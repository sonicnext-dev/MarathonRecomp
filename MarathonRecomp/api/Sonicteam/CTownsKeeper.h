#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Actor.h>
#include <Sonicteam/TownsmanManager.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class CTownsKeeper : public Actor
    {
    public:
        boost::shared_ptr<TownsmanManager::Manager> m_spTownsmanManager;
        be<float> m_Field60;
        stdx::string m_TownsmanInfoPath;
    };

    MARATHON_ASSERT_OFFSETOF(CTownsKeeper, m_spTownsmanManager, 0x58);
    MARATHON_ASSERT_OFFSETOF(CTownsKeeper, m_Field60, 0x60);
    MARATHON_ASSERT_OFFSETOF(CTownsKeeper, m_TownsmanInfoPath, 0x64);
    MARATHON_ASSERT_SIZEOF(CTownsKeeper, 0x80);
}
