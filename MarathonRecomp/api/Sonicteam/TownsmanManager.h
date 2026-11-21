#pragma once

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <Sonicteam/Game.h>
#include <Sonicteam/KynapseRandomCreator.h>

namespace Sonicteam
{
    class CTownsKeeper;

    class TownsmanManager
    {
    public:
        struct Manager
        {
            MARATHON_INSERT_PADDING(0x10);
            boost::shared_ptr<KynapseRandomCreator> spKynapseRandomCreator;
            xpointer<CTownsKeeper> pTownsKeeper;
            boost::weak_ptr<Game> wpGame;
            be<float> GenerateTime;
            be<float> DiscardTime;
            bool GenerateControl;
        };
    };

    MARATHON_ASSERT_OFFSETOF(TownsmanManager::Manager, spKynapseRandomCreator, 0x10);
    MARATHON_ASSERT_OFFSETOF(TownsmanManager::Manager, pTownsKeeper, 0x18);
    MARATHON_ASSERT_OFFSETOF(TownsmanManager::Manager, wpGame, 0x1C);
    MARATHON_ASSERT_OFFSETOF(TownsmanManager::Manager, GenerateTime, 0x24);
    MARATHON_ASSERT_OFFSETOF(TownsmanManager::Manager, DiscardTime, 0x28);
    MARATHON_ASSERT_OFFSETOF(TownsmanManager::Manager, GenerateControl, 0x2C);
    MARATHON_ASSERT_SIZEOF(TownsmanManager::Manager, 0x30);
}
