#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Prop
{
    class Scene;

    class SceneObject : public SoX::RefCountObject
    {
    public:
        xpointer<Scene> m_pScene;
    };

    MARATHON_ASSERT_OFFSETOF(SceneObject, m_pScene, 0x08);
    MARATHON_ASSERT_SIZEOF(SceneObject, 0x0C);
}
