#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdBase.h>
#include <Chao/CSD/Manager/csdmResourceBase.h>

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
        MARATHON_INSERT_PADDING(0x1C);
        RCPtr<CTexList> m_rcTexList;
        MARATHON_INSERT_PADDING(0x1C);
    };

    MARATHON_ASSERT_OFFSETOF(Project, pRootNode, 0x00);

    MARATHON_ASSERT_OFFSETOF(CProject, m_rcTexList, 0x28);
}
