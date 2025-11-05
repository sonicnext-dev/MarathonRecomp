#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/CsdLink.h>
#include <Sonicteam/CsdResource.h>

namespace Sonicteam
{
    class CsdObject : public SoX::RefCountObject, public CsdLink
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<Chao::CSD::CProject> m_pCsdProject;
        xpointer<CsdResource> m_pCsdResource;
    };

    MARATHON_ASSERT_OFFSETOF(CsdObject, m_pCsdProject, 0x24);
    MARATHON_ASSERT_OFFSETOF(CsdObject, m_pCsdResource, 0x28);
    MARATHON_ASSERT_SIZEOF(CsdObject, 0x2C);
}
