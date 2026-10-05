#pragma once

struct BufferSize
{
    uint32_t Width{};
    uint32_t Height{};
};

namespace RenderResolution
{
    BufferSize GetInternalSize();
    std::optional<BufferSize> FindBufferSize(std::string_view name, const char* pBlockName);
}
