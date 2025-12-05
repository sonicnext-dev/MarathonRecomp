#pragma once

#include <Marathon.inl>
#include <stdx/vector.h>
#include <Sonicteam/SoX/Graphics/FrameObserver.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class HairSimulator : public SoX::Graphics::FrameObserver
    {
    public:
        SoX::Math::Vector m_vField20;
        SoX::Math::Vector m_vField30;
        SoX::Math::Vector m_vField40;
        SoX::Math::Vector m_vField50;
        SoX::Math::Vector m_vField60;   
        SoX::Math::Vector m_vField70;
        SoX::Math::Vector m_vField80;
        SoX::Math::Vector m_vField90;
        SoX::Math::Vector m_vFieldA0;
        SoX::Math::Vector m_vFieldB0;
        SoX::Math::Vector m_vFieldC0;
        SoX::Math::Vector m_vFieldD0;
        xpointer<void> m_pFieldE0;
        SoX::Math::Vector m_vFieldF0;
        SoX::Math::Vector m_vField100;
        SoX::Math::Vector m_vField110;
        SoX::Math::Vector m_vField120; //not sure here
    };

    MARATHON_ASSERT_SIZEOF(HairSimulator, 0x130);

   
    //MARATHON_ASSERT_SIZEOF(ModelSonic, 0xCC);
}
