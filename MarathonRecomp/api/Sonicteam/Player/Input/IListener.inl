#pragma once

namespace Sonicteam::Player::Input
{
    inline SoX::Math::Quaternion& IListener::GetPadQuat()
    {
        auto vft = reinterpret_cast<Vftable*>(this->m_pVftable.get());
        return *GuestToHostFunction<SoX::Math::Quaternion*>(vft->fpGetPadQuat, this);
    }

    inline float IListener::GetPadMagnitude()
    {
        auto vft = reinterpret_cast<Vftable*>(this->m_pVftable.get());
        return GuestToHostFunction<float>(vft->fpGetPadMagnitude, this);
    }

    inline uint32_t IListener::GetPadButtons()
    {
        auto vft = reinterpret_cast<Vftable*>(this->m_pVftable.get());
        return GuestToHostFunction<uint32_t>(vft->fpGetPadButtons, this);
    }

    inline bool IListener::IsEnabled()
    {
        auto vft = reinterpret_cast<Vftable*>(this->m_pVftable.get());
        return GuestToHostFunction<bool>(vft->fpIsEnabled, this);
    }
}
