#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/SoX/Graphics/MaterialCommon.h>

namespace Sonicteam
{
    class MyGraphicsCommonFactory;

    class MyMaterialCommon : public SoX::Graphics::MaterialCommon
    {
    public:
        xpointer<MyGraphicsCommonFactory> m_pMyGraphicCommonFactory;
    };

    MARATHON_ASSERT_SIZEOF(MyMaterialCommon, 0x110);
    MARATHON_ASSERT_OFFSETOF(MyMaterialCommon, m_pMyGraphicCommonFactory, 0x100);
}
