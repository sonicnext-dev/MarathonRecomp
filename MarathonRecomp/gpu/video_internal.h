#pragma once

#include "video.h"

extern std::thread::id g_mainThreadId;

void DiscardTexture(GuestBaseTexture* texture, RenderTextureLayout layout);
