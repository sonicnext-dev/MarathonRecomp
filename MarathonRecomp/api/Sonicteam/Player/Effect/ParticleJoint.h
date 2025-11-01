#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Graphics/FrameObserver.h>
#include <Sonicteam/Particles/Particle.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player::Effect
{
    // Temp Name, Position
    struct ParticleLuaInfo
    {
        be<uint16_t> BankID;
        be<uint16_t> ParticleID;
        bool IsAcroarts; //kinda?
        bool IsReverse;
        be<uint32_t> Direction;
        xpointer<const char> AttachNode;  //Bone Name
        be<float> OffsetX;
        be<float> OffsetY;
        be<float> OffsetZ;
    };

    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, BankID, 0x0);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, IsAcroarts, 0x4);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, IsReverse, 0x5);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, Direction, 0x8);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, AttachNode,0xC);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, OffsetX, 0x10);
    MARATHON_ASSERT_SIZEOF(ParticleLuaInfo, 0x1C);


    class ParticleJoint : public SoX::Graphics::FrameObserver::FrameObserver
    {
    public:
        xpointer<Particles::Particle> m_pParticle;
        bool m_IsAcroarts;
        bool m_IsReverse;
        be<uint32_t> m_Direction;
        xpointer<const char> m_pAttachNode;
        SoX::Math::Vector m_Offset;
        SoX::Math::Vector m_Position;
        SoX::Math::Vector m_Rotation; //i guess
    };

    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_pParticle, 0x14);
    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_IsAcroarts, 0x18);
    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_IsReverse, 0x19);
    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_Direction, 0x1C);
    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_pAttachNode, 0x20);
    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_Offset, 0x30);
    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_Position, 0x40);
    MARATHON_ASSERT_OFFSETOF(ParticleJoint, m_Rotation, 0x50);
    MARATHON_ASSERT_SIZEOF(ParticleJoint, 0x60);
   
}
