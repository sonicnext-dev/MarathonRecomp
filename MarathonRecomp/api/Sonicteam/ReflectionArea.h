#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/ConvexShape.h>
#include <Sonicteam/SoX/Physics/PhantomListener.h>
#include <Sonicteam/SoX/RefLinkNode.h>
#include <Sonicteam/MyPhantom.h>

namespace Sonicteam
{
    class ReflectionArea : public SoX::Engine::Task
    {
    public:
        struct Vftable : public SoX::Engine::Task::Vftable
        {
            be<uint32_t> fpOnEnter;
            be<uint32_t> fpOnLeave;
        };

        class MyPhantomListener : public SoX::Physics::PhantomListener // 0x8201096C
        {
        public:
            struct MyPhantomContext
            {
                SoX::RefLinkNode<MyPhantom> rReflectionAreaPhantom;
                SoX::RefLinkNode<MyPhantom> rIntersectingPhantom;
            };
        };

        xpointer<SoX::Physics::ConvexShape> m_pConvexShape;
        SoX::Math::Vector m_Field50;
        xpointer<MyPhantom> m_pPhantom;
        bool m_IsInsideArea;
        be<float> m_ClumpCutoffCoverage;
        be<float> m_SectorCutoffCoverage;
    };

    MARATHON_ASSERT_OFFSETOF(ReflectionArea, m_pConvexShape, 0x4C);
    MARATHON_ASSERT_OFFSETOF(ReflectionArea, m_Field50, 0x50);
    MARATHON_ASSERT_OFFSETOF(ReflectionArea, m_pPhantom, 0x60);
    MARATHON_ASSERT_OFFSETOF(ReflectionArea, m_IsInsideArea, 0x64);
    MARATHON_ASSERT_OFFSETOF(ReflectionArea, m_ClumpCutoffCoverage, 0x68);
    MARATHON_ASSERT_OFFSETOF(ReflectionArea, m_SectorCutoffCoverage, 0x6C);
    MARATHON_ASSERT_SIZEOF(ReflectionArea, 0x70);
}
