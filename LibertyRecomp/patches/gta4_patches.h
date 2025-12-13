#pragma once

// =============================================================================
// GTA IV Game-Specific Patches
// =============================================================================
// These patches modify GTA IV's behavior for the recompiled PC version.
// They hook into the recompiled PPC functions using mid-asm hooks and
// function overrides.
// =============================================================================

namespace GTA4Patches
{
    // Initialize all GTA IV-specific patches
    void Init();
    
    // Update patches that need per-frame processing
    void Update(double deltaTime);
}

// =============================================================================
// FPS Patches - Unlocked framerate support
// =============================================================================
namespace GTA4FPSPatches
{
    void Init();
    
    // Fixes for physics/animation timing at high framerates
    void FixPhysicsDeltaTime(double deltaTime);
    void FixAnimationSpeed(double deltaTime);
}

// =============================================================================
// Resolution Patches - Arbitrary resolution support
// =============================================================================
namespace GTA4ResolutionPatches
{
    void Init();
    
    // Get current resolution
    uint32_t GetWidth();
    uint32_t GetHeight();
    float GetAspectRatio();
    
    // Fix UI scaling for non-16:9 resolutions
    void FixUIScale();
}

// =============================================================================
// Input Patches - Keyboard/Mouse support
// =============================================================================
namespace GTA4InputPatches
{
    void Init();
    
    // Remap controller inputs to keyboard/mouse
    void UpdateInput();
}

// =============================================================================
// Graphics Patches - Rendering fixes
// =============================================================================
namespace GTA4GraphicsPatches
{
    void Init();
    
    // Fix shadow rendering
    void FixShadows();
    
    // Fix reflection rendering  
    void FixReflections();
}

// =============================================================================
// Audio Patches - Audio system fixes
// =============================================================================
namespace GTA4AudioPatches
{
    void Init();
    
    // Initialize audio subsystem
    void InitAudio();
    
    // Update audio (called per frame)
    void UpdateAudio(double deltaTime);
}

// =============================================================================
// Save Data Patches - Save file handling
// =============================================================================
namespace GTA4SavePatches
{
    void Init();
    
    // Get save file path
    const char* GetSavePath();
}
