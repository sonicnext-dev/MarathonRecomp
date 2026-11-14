#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <Sonicteam/Prop/ActorCreators.h>
#include <Sonicteam/Prop/ClassRegistry.h>
#include <Sonicteam/Prop/GenerateArea.h>
#include <Sonicteam/Prop/Scene.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/Game.h>
#include <stdx/map.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam::Prop
{
    class Manager
    {
    public:
        xpointer<Game> m_pGame;
        MARATHON_INSERT_PADDING(0x04);
        SoX::LinkNode<Scene> m_Field08;
        SoX::LinkNode<GenerateArea> m_GenerateAreas;
        stdx::vector<boost::weak_ptr<Scene>> m_Scenes;
        stdx::map<stdx::string, be<uint32_t>> m_Field30;
        boost::weak_ptr<ActorCreators> m_wpActorCreators;
        boost::shared_ptr<ClassRegistry> m_spClassRegistry;
    };

    MARATHON_ASSERT_OFFSETOF(Manager, m_pGame, 0x00);
    MARATHON_ASSERT_OFFSETOF(Manager, m_Field08, 0x08);
    MARATHON_ASSERT_OFFSETOF(Manager, m_GenerateAreas, 0x14);
    MARATHON_ASSERT_OFFSETOF(Manager, m_Scenes, 0x20);
    MARATHON_ASSERT_OFFSETOF(Manager, m_Field30, 0x30);
    MARATHON_ASSERT_OFFSETOF(Manager, m_wpActorCreators, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Manager, m_spClassRegistry, 0x44);
    MARATHON_ASSERT_SIZEOF(Manager, 0x4C);
}
