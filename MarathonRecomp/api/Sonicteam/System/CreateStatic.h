#pragma once

namespace Sonicteam::System
{
    template <typename T, uint32_t TCreator>
    class CreateStatic
    {
    public:
        static T* Create()
        {
            return GuestToHostFunction<T*>(TCreator);
        }
    };
}
