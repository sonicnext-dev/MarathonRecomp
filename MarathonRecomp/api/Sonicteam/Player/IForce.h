#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IFlagCommunicator.h>

namespace Sonicteam::Player
{
    class IForce : public IPlugIn, public IFlagCommunicator
    {
    public:
        struct Vftable:IPlugIn::Vftable
        {
            be<uint32_t> fpGetDirection;
            be<uint32_t> fpGetForce;
        };

        be<uint32_t> m_ICommonContextIFFlag;
        be<uint32_t> m_IPostureControlFlag;
        be<uint32_t> m_PostureRequestFlag;

        SoX::Math::Vector& GetDirection();

        float GetForce();

    };

    MARATHON_ASSERT_SIZEOF(IForce, 0x30);
    MARATHON_ASSERT_OFFSETOF(IForce, m_ICommonContextIFFlag, 0x24);
    MARATHON_ASSERT_OFFSETOF(IForce, m_IPostureControlFlag, 0x28);
    MARATHON_ASSERT_OFFSETOF(IForce, m_PostureRequestFlag, 0x2C);
}

#include "IForce.inl"
