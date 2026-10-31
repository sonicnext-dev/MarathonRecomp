#pragma once

#include "video.h"

extern std::atomic<uint32_t> g_bufferUploadCount;

uint32_t ComputeTexturePitch(GuestTexture* texture);
GuestTexture* CreateTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t levels, uint32_t usage, uint32_t format, uint32_t pool, uint32_t type);
GuestSurface* CreateSurface(uint32_t width, uint32_t height, uint32_t format, uint32_t multiSample, GuestSurfaceCreateParams* params);
void DestructResource(GuestResource* resource);
void ReleaseResource(GuestResource* resource);
