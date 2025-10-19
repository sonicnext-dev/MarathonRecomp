#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdBase.h>
#include <Chao/CSD/Manager/csdmResourceBase.h>
#include <Chao/CSD/Manager/csdmNodeObserver.h>
#include <Chao/CSD/Manager/csdmSubjectBase.h>

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
