#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class StageMap : public SoX::Component
    {
    public:
        stdx::string m_Name;
        stdx::string m_Text;
        stdx::vector<xpointer<StageMap>> m_Children;
    };

    MARATHON_ASSERT_OFFSETOF(StageMap, m_Name, 0x20);
    MARATHON_ASSERT_OFFSETOF(StageMap, m_Text, 0x3C);
    MARATHON_ASSERT_OFFSETOF(StageMap, m_Children, 0x58);
    MARATHON_ASSERT_SIZEOF(StageMap, 0x68);
}
