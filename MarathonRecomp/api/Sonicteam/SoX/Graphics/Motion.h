#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>


namespace Sonicteam::SoX::Graphics
{
    class MotionMgr;

    class Motion : public IResource2<Motion, MotionMgr>
    {
    };
}
