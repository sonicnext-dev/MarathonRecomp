#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam
{
    class CsdObject : public SoX::RefCountObject, public CsdLink
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<Chao::CSD::CProject> pCsdProject;
        MARATHON_INSERT_PADDING(4);
    };
}
