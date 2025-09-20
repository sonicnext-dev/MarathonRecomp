#pragma once

#include <Marathon.inl>
#include <Sonicteam/Actor.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class NamedActor : public Actor
    {
    public:
        stdx::string m_Name;
    };
}
