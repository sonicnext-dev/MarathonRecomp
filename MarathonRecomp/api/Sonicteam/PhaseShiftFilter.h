#pragma once

#include <Marathon.inl>
#include <Sonicteam/Postprocess.h>

namespace Sonicteam
{
    class PhaseShiftFilter : public Postprocess
    {
    public:
        be<float> m_Field1C;
    };

    MARATHON_ASSERT_OFFSETOF(PhaseShiftFilter, m_Field1C, 0x1C);
    MARATHON_ASSERT_SIZEOF(PhaseShiftFilter, 0x20);
}
