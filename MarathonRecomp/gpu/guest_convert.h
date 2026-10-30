#pragma once

#include <plume_render_interface.h>

plume::RenderBlend ConvertBlendMode(uint32_t blendMode);
plume::RenderBlendOperation ConvertBlendOp(uint32_t blendOp);
plume::RenderComparisonFunction ConvertCompareFunc(uint32_t compareFunc);
plume::RenderStencilOp ConvertStencilOp(uint32_t stencilOp);
plume::RenderFormat ConvertFormat(uint32_t format);
plume::RenderTextureAddressMode ConvertTextureAddressMode(size_t value);
plume::RenderFilter ConvertTextureFilter(uint32_t value);
plume::RenderBorderColor ConvertBorderColor(uint32_t value);
plume::RenderPrimitiveTopology ConvertPrimitiveType(uint32_t primitiveType);
const char* ConvertDeclUsage(uint32_t usage);
plume::RenderFormat ConvertDeclType(uint32_t type);
