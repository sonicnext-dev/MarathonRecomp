#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Camera
{
    class Cameraman : Actor
    {
    public:
        MARATHON_INSERT_PADDING(0x58);
        be<float> m_FOV;
    };
}
