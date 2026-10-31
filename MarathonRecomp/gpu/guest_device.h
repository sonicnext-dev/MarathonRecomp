#pragma once

#include "video.h"

void ResetIntermediaryUploadAllocator();
void SetRenderTarget(GuestDevice* device, uint32_t index, GuestSurface* renderTarget);
void SetDepthStencilSurface(GuestDevice* device, GuestSurface* depthStencil);
