#pragma once

#include <Marathon.inl>
#include <Sonicteam/PropFixture.h>

namespace Sonicteam
{
    class CommonObjectHint : public PropFixture
    {
    public:
        enum CommonObjectHintType : uint32_t
        {
            CommonObjectHintType_HintRing,
            CommonObjectHintType_HintVolume
        };

        MARATHON_INSERT_PADDING(4);
        char m_MessageName[20];
        MARATHON_INSERT_PADDING(0x40);
        be<CommonObjectHintType> m_Type;
        bool m_IsFastAnim;
        bool m_IsHitAnim;
        MARATHON_INSERT_PADDING(2);
        be<float> m_HitTime;
    };

    MARATHON_ASSERT_OFFSETOF(CommonObjectHint, m_MessageName, 0x184);
    MARATHON_ASSERT_OFFSETOF(CommonObjectHint, m_Type, 0x1D8);
    MARATHON_ASSERT_OFFSETOF(CommonObjectHint, m_IsFastAnim, 0x1DC);
    MARATHON_ASSERT_OFFSETOF(CommonObjectHint, m_IsHitAnim, 0x1DD);
    MARATHON_ASSERT_OFFSETOF(CommonObjectHint, m_HitTime, 0x1E0);
}
