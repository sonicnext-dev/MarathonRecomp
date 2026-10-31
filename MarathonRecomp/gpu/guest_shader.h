#pragma once

#include "video.h"

extern std::unique_ptr<GuestShader> g_conditionalSurveyPSShader;

void LoadEmbeddedResources();
void CreateReplacementShaders();
GuestShader* GetOrCreateShader(XXH64_hash_t hash, ResourceType resourceType);
RenderShader* GetReplacementShader(GuestShader* guestShader);
RenderShader* GetOrLinkShader(GuestShader* guestShader, uint32_t specConstants);
