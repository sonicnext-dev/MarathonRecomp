#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDLoading : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        enum HUDLoadingFlags
        {
            HUDLoadingFlags_Finished = 6,
            HUDLoadingFlags_Open = 0x200,
            HUDLoadingFlags_End = 0x400
        };

        MARATHON_INSERT_PADDING(0x5C);
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(8);
    };
}
