#pragma once

#include <api/Marathon.h>

constexpr float DevTitleDelayDefault = 1.0;
static float DevTitleDelay = DevTitleDelayDefault;

struct StageMapSelectorConfig {
    // Layout
    float windowWidthRatio = 0.8f;
    float buttonHeightRatio = 0.04f;
    float leading = 1.25f;
    int visibleItems = 16;

    // Positioning
    ImVec2 paddingRatio = { 0.05f, 0.1f };

    // Animation
    float scrollSpeed = 16.0f;
};

static StageMapSelectorConfig s_StageMapSelectorConfig;
static float s_StageMapScroll = 0.0f;
static const void* s_pLastStageMap = nullptr;

struct DevMessage
{
    be<uint32_t> id;
    be<uint32_t> id2;
    uint8_t id3;
};

class DevTitleMenu
{
public:
    static bool IsVisible;

    static void Init();
    static void Draw();

    static void AudioCommon(DevMessage* message);

    static void Close();
    static bool CanClose();
};
