#pragma once

namespace Sonicteam::Player
{
    inline SoX::Math::Vector& IForce::GetDirection()
    {
        auto vft = reinterpret_cast<Vftable*>(static_cast<IPlugIn*>(this)->m_pVftable.get());
        return *GuestToHostFunction<SoX::Math::Vector*>(vft->fpGetDirection, this);
    }

    inline float IForce::GetForce()
    {
        auto vft = reinterpret_cast<Vftable*>(static_cast<IPlugIn*>(this)->m_pVftable.get());
        return GuestToHostFunction<double>(vft->fpGetForce, this);
    }
}
