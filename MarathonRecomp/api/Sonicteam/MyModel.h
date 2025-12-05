#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/SoX/Graphics/ModelCommon.h>

namespace Sonicteam
{
    class MyGraphicDevice;
    class MyModelStrategyNN;

    class MyModel : public SoX::Graphics::ModelCommon
    {
    public:
        xpointer<MyGraphicDevice> m_pMyGraphicDevice;
        xpointer<MyModelStrategyNN> m_MyModelStrategyNN;
    };

    MARATHON_ASSERT_SIZEOF(MyModel, 0x88);
    MARATHON_ASSERT_OFFSETOF(MyModel, m_pMyGraphicDevice, 0x80);
    MARATHON_ASSERT_OFFSETOF(MyModel, m_MyModelStrategyNN, 0x84);
}
