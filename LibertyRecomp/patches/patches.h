#pragma once

// GTA IV-specific patches
#include "gta4_patches.h"

// Note: The following Sonic '06 specific patches are disabled for GTA IV:
// - aspect_ratio_patches.h (has Sonicteam namespace references)
// - camera_patches.h (has Sonicteam namespace references)
// - Other game-specific patches

inline void InitPatches()
{
    // Initialize GTA IV-specific patches
    GTA4Patches::Init();
}
