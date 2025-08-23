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
    };
}
