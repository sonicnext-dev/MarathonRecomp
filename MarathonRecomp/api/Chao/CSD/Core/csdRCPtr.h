#pragma once

#include <Chao/CSD/Core/csdRCPtrAbs.h>

namespace Chao::CSD
{
    template<typename T>
    class RCPtr : RCPtrAbs
    {
    public:
        class RCObjectImp;
    };
}
