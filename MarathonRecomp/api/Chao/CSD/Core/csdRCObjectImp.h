#pragma once

#include <Chao/CSD/Core/csdRCPtr.h>

namespace Chao::CSD
{
    template<typename T>
    class RCPtr<T>::RCObjectImp : public RCObject {};
}
