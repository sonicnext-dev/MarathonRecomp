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
        stdx::vector<xpointer<StageMap>> m_vpStageMap;

    };
}
