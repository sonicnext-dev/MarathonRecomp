#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpanBASE.h>

namespace Sonicteam::Spanverse
{
    class SpanACBS : public SpanBASE
    {
    public:
        static const uint32_t Header = 0x41434253;
    };
}
