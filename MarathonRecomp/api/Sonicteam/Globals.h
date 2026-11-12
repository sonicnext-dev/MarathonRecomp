#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/ShadowMapConfig.h>
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

        static inline SoX::Array<be<float>, 9>* ms_pMainDisplayColors{};

        static inline SoX::Array<be<float>, 4>* ms_pShadowCascadePlaneDistances{};
        static inline ShadowMapConfig* ms_pShadowMapConfigDefault{};
        static inline ShadowMapConfig* ms_pShadowMapConfig{};
        static inline ShadowMapConfig* ms_pShadowMapConfigAlt{};
        static inline bool* ms_pIsShadowMapConfigAlt{};

        static void Init()
        {
            ms_pCurrentRenderScript = reinterpret_cast<stdx::string*>(MmGetHostAddress(0x82B814F8));
            ms_pMainDisplayColors = reinterpret_cast<SoX::Array<be<float>, 9>*>(MmGetHostAddress(0x82036BE4));
            ms_pShadowCascadePlaneDistances = reinterpret_cast<SoX::Array<be<float>, 4>*>(MmGetHostAddress(0x8204D228));
            ms_pShadowMapConfigDefault = reinterpret_cast<ShadowMapConfig*>(MmGetHostAddress(0x82B84B20));
            ms_pShadowMapConfig = reinterpret_cast<ShadowMapConfig*>(MmGetHostAddress(0x82B84B30));
            ms_pShadowMapConfigAlt = reinterpret_cast<ShadowMapConfig*>(MmGetHostAddress(0x82B84B50));
            ms_pIsShadowMapConfigAlt = reinterpret_cast<bool*>(MmGetHostAddress(0x82D3C070));
        }
    };
}
