#pragma once

#include <Marathon.inl>
#include <CSD/Manager/csdmBase.h>
#include <CSD/Manager/csdmResourceBase.h>
#include <CSD/Manager/csdmNodeObserver.h>
#include <CSD/Manager/csdmSubjectBase.h>

namespace Chao::CSD
{
    struct Node;

    class CNode : public CResourceBase<Node>, SubjectBase<CNodeObserver, CNode>, CBase
    {
    public:
        MARATHON_INSERT_PADDING(0x34);
        xpointer<CMotionPattern> m_pMotionPattern;
        MARATHON_INSERT_PADDING(0x18);
    };
}
