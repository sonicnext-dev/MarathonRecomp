#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
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
        static inline be<float>* ms_MainDisplayColours[9];

        static void Init()
        {
            for (int i = 0; i < 9; i++)
                ms_MainDisplayColours[i] = (be<float>*)MmGetHostAddress(0x82036BE4 + (i * 4));
        }
    };
}
