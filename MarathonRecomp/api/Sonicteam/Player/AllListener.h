#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListenerTemplate.h>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>
#include <Sonicteam/SoX/Physics/ShapeCastEvent.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class AllListener : public ICollisionListenerTemplate<SoX::Physics::ShapeCastListener, SoX::Physics::ShapeCastEvent> 
    {
    public:
        SoX::Math::Vector m_Position; 
        SoX::Math::Vector m_Vector90; 
        be<float> m_Radius;
        uint8_t m_FieldA4; // Flag, Type , Mode ????, 1 or 0
        SoX::Math::Vector m_aContactNormals[6]; // m_CurrentSurfaceContacts, all contacts?
        be<uint32_t> m_Field110;
    };

    MARATHON_ASSERT_SIZEOF(AllListener, 0x120);
    MARATHON_ASSERT_OFFSETOF(AllListener, m_Position, 0x80);
    MARATHON_ASSERT_OFFSETOF(AllListener, m_Vector90, 0x90);
    MARATHON_ASSERT_OFFSETOF(AllListener, m_Radius, 0xA0);
    MARATHON_ASSERT_OFFSETOF(AllListener, m_FieldA4, 0xA4);
    MARATHON_ASSERT_OFFSETOF(AllListener, m_aContactNormals, 0xB0);
    MARATHON_ASSERT_OFFSETOF(AllListener, m_Field110, 0x110);
}
