#pragma once

#include <Marathon.inl>
#include <stdx/string.h>

namespace Sonicteam::SoX
{
    class IResource : public RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(4);
        stdx::string m_FilePath;
        MARATHON_INSERT_PADDING(0x38);
    };
}
