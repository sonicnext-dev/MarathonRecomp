#pragma once

#include <CSD/Manager/csdmRCPtrAbs.h>

namespace Chao::CSD
{
    template<typename T>
    class RCPtr : RCPtrAbs
    {
    public:
        class RCObjectImp;
    };
}
