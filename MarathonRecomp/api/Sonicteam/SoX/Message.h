#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX
{
    struct IMessage
    {
        be<uint32_t> ID{};

        IMessage() {}
        IMessage(const uint32_t id) : ID(id) {}
    };

    template <const uint32_t id>
    struct Message : IMessage
    {
        Message()
        {
            ID = id;
        }

        static const uint32_t GetID()
        {
            return id;
        }
    };
}
