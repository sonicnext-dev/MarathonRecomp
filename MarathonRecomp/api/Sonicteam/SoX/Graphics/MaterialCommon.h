#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Model.h>
#include <stdx/map.h>
#include <stdx/string.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/Math/Vector.h>


namespace Sonicteam::SoX::Graphics
{
    class SamplerCommon;


    class MaterialCommon
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x1C);

        struct MaterialParams
        {
            union
            {
                SoX::Math::Vector Params[8];
                struct
                {
                    SoX::Math::Vector m_Color;
                    SoX::Math::Vector m_Emission;
                    SoX::Math::Vector m_Emission2;
                    SoX::Math::Vector m_Fresnel;
                    SoX::Math::Vector m_Specular;
                    SoX::Math::Vector m_Field70;
                    SoX::Math::Vector m_Field80;
                    SoX::Math::Vector m_Field90;
                };
            };
        };

        MaterialParams m_Params;
        MARATHON_INSERT_PADDING(0x18);
        be<uint32_t> m_CullMode; // DoubleSide, Normal, Reverse, DoubleSide
        boost::shared_ptr<SamplerCommon> m_aspSampler[4];
        MARATHON_INSERT_PADDING(4);
        boost::shared_ptr<void> m_aspFieldE0[2];
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_OFFSETOF(MaterialCommon, m_Params, 0x20);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon, m_CullMode, 0xB8);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon, m_aspSampler, 0xBC);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon, m_aspFieldE0, 0xE0);
    MARATHON_ASSERT_SIZEOF(MaterialCommon, 0x100);

    // MaterialParams
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Color, 0x0);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Emission, 0x10);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Emission2, 0x20);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Fresnel, 0x30);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Specular, 0x40);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Field70, 0x50);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Field80, 0x60);
    MARATHON_ASSERT_OFFSETOF(MaterialCommon::MaterialParams, m_Field90, 0x70);

    
}
