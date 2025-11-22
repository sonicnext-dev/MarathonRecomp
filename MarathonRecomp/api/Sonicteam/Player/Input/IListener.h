#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Input/Listener.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Math/Math.h>

namespace Sonicteam::Player::Input
{
    class IListener : public SoX::Input::Listener
    {
    public:
        struct Vftable: public SoX::Object::Vftable
        {
            be<uint32_t> fpStep;
            be<uint32_t> fpGetPadQuat;
            be<uint32_t> fpGetPadMagnitude;
            be<uint32_t> fpGetPadButtons;
            be<uint32_t> fpIsEnabled;
        };

        SoX::Math::Quaternion& GetPadQuat();

        float GetPadMagnitude();

        uint32_t GetPadButtons();

        bool IsEnabled();
    };
}

#include "IListener.inl"
