#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Graphics
{
    class FrameGP : public Frame
    {
    public:
        MARATHON_INSERT_PADDING(0x84);
        stdx::string m_Name;
    };
}
