#pragma once

#include <Marathon.inl>
#include <CSD/Manager/csdmBase.h>
#include <CSD/Manager/csdmResourceBase.h>

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
}
