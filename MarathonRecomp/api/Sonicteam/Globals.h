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
        static inline be<float>* ms_MainDisplayColours[9];

        static inline stdx::list<MyRenderProcess>* ms_lMyRenderProcess;

        static inline stdx::string* ms_CurrentRenderScript;

        static inline const char* ms_strBuild;             // Build
        static inline const char* ms_strRenderSpanverse;   // RenderSpanverse
        static inline const char* ms_strRenderGE1Particle; // RenderGE1Particle


        static void Init()
        {
            for (int i = 0; i < 9; i++)
                ms_MainDisplayColours[i] = (be<float>*)MmGetHostAddress(0x82036BE4 + (i * 4));

            ms_lMyRenderProcess = (stdx::list<MyRenderProcess>*)MmGetHostAddress(0x82D3C568);

            ms_CurrentRenderScript = (stdx::string*)MmGetHostAddress(0x82B814F8);

            ms_strBuild = (const char*)MmGetHostAddress(0x82024000);
            ms_strRenderSpanverse = (const char*)MmGetHostAddress(0x8204C0C8);
            ms_strRenderGE1Particle = (const char*)MmGetHostAddress(0x8204C05C);
        }
    };
}
