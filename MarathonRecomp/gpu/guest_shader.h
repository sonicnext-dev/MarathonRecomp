#pragma once

#include "video.h"

GuestShader* GetOrCreateShader(XXH64_hash_t hash, ResourceType resourceType);
