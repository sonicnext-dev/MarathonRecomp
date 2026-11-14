#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/AllocatedData.h>
#include <Sonicteam/Prop/Group.h>
#include <Sonicteam/Prop/Instance.h>
#include <Sonicteam/Prop/SceneData.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <stdx/vector.h>

namespace Sonicteam::Prop
{
    class Manager;

    class Scene
    {
    public:
        xpointer<Manager> m_pManager;
        SoX::LinkNode<Scene> m_Field04;
        SoX::LinkNode<Scene> m_Field10;
        xpointer<SceneData> m_pSceneData;
        MARATHON_INSERT_PADDING(0x04);
        xpointer<AllocatedData> m_pAllocatedData;
        stdx::vector<xpointer<Instance>> m_Instances;
        stdx::vector<xpointer<Group>> m_Groups;
        MARATHON_INSERT_PADDING(0x60);
    };

    MARATHON_ASSERT_OFFSETOF(Scene, m_pManager, 0x00);
    MARATHON_ASSERT_OFFSETOF(Scene, m_Field04, 0x04);
    MARATHON_ASSERT_OFFSETOF(Scene, m_Field10, 0x10);
    MARATHON_ASSERT_OFFSETOF(Scene, m_pSceneData, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Scene, m_pAllocatedData, 0x24);
    MARATHON_ASSERT_OFFSETOF(Scene, m_Instances, 0x28);
    MARATHON_ASSERT_OFFSETOF(Scene, m_Groups, 0x38);
    MARATHON_ASSERT_SIZEOF(Scene, 0xA8);
}
