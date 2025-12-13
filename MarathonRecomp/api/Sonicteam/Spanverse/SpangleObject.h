#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleObject.h>
#include <stdx/map.h>
#include <stdx/string.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/GE1PE/RefCountObject.h>

namespace Sonicteam::Spanverse
{
    class SpangleObject : public SpkSpangleObject
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53424F42; // SBOB

        stdx::map<be<uint32_t>, be<uint64_t>> m_FieldB0;
        stdx::map<be<uint32_t>, be<uint32_t>> m_FieldBC;
        stdx::map<stdx::string, SoX::RefSharedPointer<GE1PE::RefCountObject>> m_FieldC8;
        MARATHON_INSERT_PADDING(0x5C);

    };

    MARATHON_ASSERT_OFFSETOF(SpangleObject, m_FieldB0, 0xB0);
    MARATHON_ASSERT_OFFSETOF(SpangleObject, m_FieldBC, 0xBC);
    MARATHON_ASSERT_OFFSETOF(SpangleObject, m_FieldC8, 0xC8);
    MARATHON_ASSERT_SIZEOF(SpangleObject, 0x130);
}
