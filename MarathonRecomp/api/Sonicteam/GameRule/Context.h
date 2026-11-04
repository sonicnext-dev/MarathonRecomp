#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Array.h>
#include <stdx/string.h>

namespace Sonicteam::GameRule
{
    class Context : public SoX::RefCountObject
    {
    public:
        enum GameMode
        {
            GameMode_None = 0,
            GameMode_Tag = 1,
            GameMode_Battle = 2
        };

        be<GameMode> m_GameMode;
        be<uint32_t> m_PlayerCount;
        SoX::Array<stdx::string, 4> m_PlayerNames;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_OFFSETOF(Context, m_GameMode, 0x08);
    MARATHON_ASSERT_OFFSETOF(Context, m_PlayerCount, 0x0C);
    MARATHON_ASSERT_OFFSETOF(Context, m_PlayerNames, 0x10);
    MARATHON_ASSERT_SIZEOF(Context, 0x88);
}
