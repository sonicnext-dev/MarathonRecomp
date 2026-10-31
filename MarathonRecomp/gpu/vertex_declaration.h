#pragma once

#include "video.h"

GuestVertexDeclaration* CreateVertexDeclarationWithoutAddRef(GuestVertexElement* vertexElements);
GuestVertexDeclaration* FindVertexDeclaration(XXH64_hash_t hash);
