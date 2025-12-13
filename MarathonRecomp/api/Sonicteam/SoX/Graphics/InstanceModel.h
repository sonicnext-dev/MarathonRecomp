#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Scenery/CameraEventCallback.h>
#include <Sonicteam/SoX/Graphics/FrameObserver.h>
#include <Sonicteam/SoX/Graphics/Node.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::SoX::Graphics
{
    class InstanceModel : public RefCountObject, public Scenery::CameraEventCallback
    {
    public:
        class frameObserver : public FrameObserver
        {
            xpointer<InstanceModel> m_pParent;
        };

        frameObserver m_frameObserver;
        RefSharedPointer<Node> m_spNode;
        MARATHON_INSERT_PADDING(8);
        Math::Vector m_Position;
        Math::Vector m_Position2;
        bool m_Field50;
    };

    MARATHON_ASSERT_OFFSETOF(InstanceModel, m_frameObserver, 0xC);
    MARATHON_ASSERT_OFFSETOF(InstanceModel, m_spNode, 0x24);
    MARATHON_ASSERT_OFFSETOF(InstanceModel, m_Position, 0x30);
    MARATHON_ASSERT_OFFSETOF(InstanceModel, m_Position2, 0x40);
    MARATHON_ASSERT_OFFSETOF(InstanceModel, m_Field50, 0x50);
    MARATHON_ASSERT_SIZEOF(InstanceModel, 0x60);
}
