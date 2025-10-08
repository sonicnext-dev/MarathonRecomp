#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/ObjectPsiContext.h>
#include <Sonicteam/SoX/AI/StateMachine.h>
#include <api/stdx/vector.h>
#include <boost/smart_ptr/shared_ptr.h>


namespace Sonicteam
{
    class PsiContext;

    class ObjectPsi : public SoX::MessageReceiver
    {
    public:
        SoX::AI::StateMachine<PsiContext> m_StateMachine;
        xpointer<ObjectPsiContext> m_pContext;
    };

    MARATHON_ASSERT_SIZEOF(ObjectPsi, 0x28);
    MARATHON_ASSERT_OFFSETOF(ObjectPsi, m_StateMachine, 4);
    MARATHON_ASSERT_OFFSETOF(ObjectPsi, m_pContext, 0x24);
}
