#pragma once

#include <filesystem>
#include <functional>
#include <vector>
#include <cstdint>

struct ShaderConversionJournal
{
    enum class Result
    {
        Success,
        Cancelled,
        NoShadersFound,
        ExtractionFailed,
        CompilationFailed,
        CacheWriteFailed
    };

    uint64_t shadersProcessed = 0;
    uint64_t shadersTotal = 0;
    uint64_t bytesWritten = 0;
    Result lastResult = Result::Success;
    std::string lastErrorMessage;
};

enum class ShaderPlatform
{
    DXIL,    // Windows (Direct3D 12)
    SPIRV,   // Linux/Windows (Vulkan)
    AIR,     // macOS (Metal)
    All      // Generate all formats
};

struct ShaderConverter
{
    // Detect the current platform and return appropriate shader format
    static ShaderPlatform detectPlatform();
    
    // Convert all shaders from the game installation
    // gameDirectory: Path to installed game (contains shaders/)
    // cacheDirectory: Where to store compiled shaders
    // platform: Target shader platform
    // journal: Progress tracking
    // progressCallback: Called periodically, return false to cancel
    static bool convertShaders(
        const std::filesystem::path& gameDirectory,
        const std::filesystem::path& cacheDirectory,
        ShaderPlatform platform,
        ShaderConversionJournal& journal,
        const std::function<bool()>& progressCallback
    );
    
    // Scan for .fxc files in a directory
    static std::vector<std::filesystem::path> findShaderFiles(
        const std::filesystem::path& directory
    );
    
    // Extract Xbox 360 shader binaries from a RAGE FXC file
    static bool extractShadersFromFxc(
        const std::filesystem::path& fxcPath,
        std::vector<std::pair<std::string, std::vector<uint8_t>>>& extractedShaders,
        ShaderConversionJournal& journal
    );
    
    // Check if shader cache exists and is valid
    static bool isCacheValid(
        const std::filesystem::path& cacheDirectory,
        const std::filesystem::path& gameDirectory
    );
};
