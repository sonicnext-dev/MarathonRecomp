#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Color.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/ISceneParam.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class SceneLightParam : public ISceneParam
    {
    public:
        struct AmbientLight
        {
            SoX::RGBAF32 Color;
        };

        struct DirectionalLight
        {
            SoX::RGBAF32 Color;
            SoX::Math::Vector Direction;
        };

        class IUserLight : public SoX::RefCountObject {};

        SoX::LinkNode<SceneLightParam> m_Siblings;
        be<uint32_t> m_Field14;
        MARATHON_INSERT_PADDING(0x0C); // stdx::map<?, ?>
        MARATHON_INSERT_PADDING(0x0C); // stdx::map<?, ?>
        stdx::map<stdx::string, SoX::RefSharedPointer<ISceneParam>> m_Children;
        MARATHON_INSERT_PADDING(0x44);
    };

    MARATHON_ASSERT_OFFSETOF(SceneLightParam::AmbientLight, Color, 0x00);
    MARATHON_ASSERT_SIZEOF(SceneLightParam::AmbientLight, 0x10);

    MARATHON_ASSERT_OFFSETOF(SceneLightParam::DirectionalLight, Color, 0x00);
    MARATHON_ASSERT_OFFSETOF(SceneLightParam::DirectionalLight, Direction, 0x10);
    MARATHON_ASSERT_SIZEOF(SceneLightParam::DirectionalLight, 0x20);

    MARATHON_ASSERT_SIZEOF(SceneLightParam::IUserLight, 0x08);

    MARATHON_ASSERT_OFFSETOF(SceneLightParam, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneLightParam, m_Field14, 0x14);
    MARATHON_ASSERT_OFFSETOF(SceneLightParam, m_Children, 0x30);
    MARATHON_ASSERT_SIZEOF(SceneLightParam, 0x80);
}
