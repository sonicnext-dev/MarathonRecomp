#pragma once

#include <Marathon.inl>
#include <stdx/list.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class MyRenderProcess;

    enum Character : uint32_t
    {
        Character_Sonic,
        Character_Shadow,
        Character_Silver,
        Character_Tails,
        Character_Amy,
        Character_Knuckles,
        Character_Omega,
        Character_Rouge,
        Character_Blaze
    };

    struct Globals
    {
        static inline stdx::string* ms_pCurrentRenderScript{};

        static inline be<float>* ms_apMainDisplayColours[9]{};

        static void Init()
        {
            ms_pCurrentRenderScript = reinterpret_cast<stdx::string*>(MmGetHostAddress(0x82B814F8));

            for (int i = 0; i < 9; i++)
                ms_apMainDisplayColours[i] = reinterpret_cast<be<float>*>(MmGetHostAddress(0x82036BE4 + (i * 4)));
        }
    };
}
