#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpanBASE.h>

namespace Sonicteam::Spanverse
{
    class SpanACBS : public SpanBASE
    {
    public:
        static constexpr uint32_t ms_Signature = 0x41434253; // ACBS
    };
}
