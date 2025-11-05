#pragma once

#include <Marathon.inl>
#include <Sonicteam/StdImageFilters/SingleTechniqueFilter.h>

namespace Sonicteam::StdImageFilters
{
    class BurnoutBlurFilter : public SingleTechniqueFilter
    {
    public:
        MARATHON_INSERT_PADDING(0x14);
        be<float> m_Magnitude;
        xpointer<SoX::Graphics::TechniqueFXL> m_p1xBlurTechnique;
        xpointer<SoX::Graphics::TechniqueFXL> m_p4xBlurTechnique;
        xpointer<SoX::Graphics::TechniqueFXL> m_p8xBlurTechnique;
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(BurnoutBlurFilter, m_Magnitude, 0x2C);
    MARATHON_ASSERT_OFFSETOF(BurnoutBlurFilter, m_p1xBlurTechnique, 0x30);
    MARATHON_ASSERT_OFFSETOF(BurnoutBlurFilter, m_p4xBlurTechnique, 0x34);
    MARATHON_ASSERT_OFFSETOF(BurnoutBlurFilter, m_p8xBlurTechnique, 0x38);
    MARATHON_ASSERT_SIZEOF(BurnoutBlurFilter, 0x40);
}
