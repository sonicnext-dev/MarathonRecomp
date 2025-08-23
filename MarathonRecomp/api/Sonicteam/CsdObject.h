#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
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
}
