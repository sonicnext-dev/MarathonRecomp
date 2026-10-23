#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdBase.h>
#include <Chao/CSD/Manager/csdmResourceBase.h>
#include <stdx/map.h>

namespace Chao::CSD
{
    class SceneNode;
    class CProject;
    class CScene;
    class CTexList;

    struct Project
    {
        xpointer<SceneNode> pRootNode;
    };

    class CProject : public CResourceBase<Project>, CBase
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
        stdx::map<xpointer<const char>, xpointer<RCObject<CScene>>> m_Scenes;
        stdx::map<float, xpointer<RCObject<CScene>>> m_SortOrder;
        xpointer<CTexList> m_pTexList;
        MARATHON_INSERT_PADDING(0x1C);
    };

    MARATHON_ASSERT_OFFSETOF(Project, pRootNode, 0x00);

    MARATHON_ASSERT_OFFSETOF(CProject, m_Scenes, 0x10);
    MARATHON_ASSERT_OFFSETOF(CProject, m_SortOrder, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CProject, m_pTexList, 0x28);
}
