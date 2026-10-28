#pragma once

#include <sstream>
#include <string>
#include <vector>
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

        std::string GetHierarchy()
        {
            std::stringstream result{};
            std::vector<const char*> hierarchy{};

            auto pStageMap = this;

            while (pStageMap)
            {
                if (pStageMap->m_Name.empty())
                    break;

                hierarchy.push_back(pStageMap->m_Name.c_str());

                pStageMap = pStageMap->GetParent<Sonicteam::StageMap>();
            }

            for (int i = hierarchy.size() - 1; i >= 0; i--)
                result << "/" << hierarchy[i];

            return result.str();
        }

        int GetTotalChildCount()
        {
            int result = m_Children.size();

            for (const auto& rChild : m_Children)
                result += rChild->m_Children.size();

            return result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(StageMap, m_Name, 0x20);
    MARATHON_ASSERT_OFFSETOF(StageMap, m_Text, 0x3C);
    MARATHON_ASSERT_OFFSETOF(StageMap, m_Children, 0x58);
    MARATHON_ASSERT_SIZEOF(StageMap, 0x68);
}
