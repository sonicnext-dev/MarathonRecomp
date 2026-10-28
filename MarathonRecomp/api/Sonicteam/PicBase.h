#pragma once

#include <Marathon.inl>
#include <Sonicteam/CsdObject.h>

namespace Sonicteam
{
    class PicBase
    {
    public:
        enum PicState : uint32_t
        {
            PicState_Load,
            PicState_Init,
            PicState_Start,
            PicState_TitleWait,
            PicState_TitleOutro,
            PicState_TitleProceed,
            PicState_Next
        };

        xpointer<void> m_pVftable;
        be<PicState> m_PrevState;
        be<PicState> m_NextState;
        be<PicState> m_State;
        xpointer<CsdObject> m_pCsdObject;
    };

    MARATHON_ASSERT_OFFSETOF(PicBase, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(PicBase, m_PrevState, 0x04);
    MARATHON_ASSERT_OFFSETOF(PicBase, m_NextState, 0x08);
    MARATHON_ASSERT_OFFSETOF(PicBase, m_State, 0x0C);
    MARATHON_ASSERT_OFFSETOF(PicBase, m_pCsdObject, 0x10);
    MARATHON_ASSERT_SIZEOF(PicBase, 0x14);
}
