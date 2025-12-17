#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

/**
 * RPF (RAGE Package File) extraction for GTA IV.
 * 
 * RPF2 Format (GTA IV):
 * - Header is ALWAYS plaintext (never encrypted)
 * - TOC is ALWAYS plaintext (starts at offset 0x800)
 * - Encryption is per-file (AES-256-ECB, no IV, no padding)
 * - Some files may be compressed (zlib)
 * 
 * This module supports two approaches:
 * 1. Native C++ parsing (preferred, no external dependencies)
 * 2. SparkCLI wrapper (fallback, requires Mono on non-Windows)
 */
namespace RpfExtractor
{
    struct RpfHeader
    {
        uint32_t magic;          // 0x52504632 ("RPF2") for GTA IV
        uint32_t tocSize;
        uint32_t entryCount;
        uint32_t unknown;
        uint32_t encrypted;      // 0 = unencrypted files, non-zero = some files encrypted
    };
    
    struct RpfFileEntry
    {
        std::string name;
        uint32_t offset;
        uint32_t size;
        uint32_t compressedSize;
        uint8_t resourceType;
        bool isCompressed;
        bool isEncrypted;
        bool isDirectory;
    };
    
    struct ExtractionResult
    {
        bool success = false;
        std::string errorMessage;
        std::filesystem::path extractedPath;
        uint64_t filesExtracted = 0;
        uint64_t bytesExtracted = 0;
    };
    
    struct ShaderScanResult
    {
        std::vector<std::filesystem::path> fxcFiles;
        uint64_t totalShaderBytes = 0;
    };
    
    /**
     * Check if a file is a valid RPF archive.
     */
    bool IsRpfFile(const std::filesystem::path& path);
    
    /**
     * Read the RPF header from a file.
     */
    bool ReadHeader(const std::filesystem::path& path, RpfHeader& header);
    
    /**
     * Check if an RPF archive contains encrypted files.
     * Note: Header/TOC are never encrypted, only file data.
     */
    bool HasEncryptedFiles(const std::filesystem::path& path);
    
    /**
     * Load AES key from file or extract from executable.
     * Key file should be 32 bytes (256-bit AES key).
     */
    bool LoadAesKey(const std::filesystem::path& keyPath, std::vector<uint8_t>& key);
    
    /**
     * Extract AES key from GTAIV executable (default.xex or GTAIV.exe).
     */
    bool ExtractAesKeyFromExecutable(
        const std::filesystem::path& exePath,
        std::vector<uint8_t>& key
    );
    
    /**
     * List all entries in an RPF archive.
     */
    std::vector<RpfFileEntry> ListEntries(
        const std::filesystem::path& rpfPath,
        const std::vector<uint8_t>& aesKey = {}
    );
    
    /**
     * Extract all contents of an RPF archive.
     */
    ExtractionResult Extract(
        const std::filesystem::path& rpfPath,
        const std::filesystem::path& outputDir,
        const std::vector<uint8_t>& aesKey = {},
        const std::function<void(float)>& progressCallback = nullptr
    );
    
    /**
     * Extract only shader files (.fxc) from an RPF archive.
     */
    ExtractionResult ExtractShaders(
        const std::filesystem::path& rpfPath,
        const std::filesystem::path& outputDir,
        const std::vector<uint8_t>& aesKey = {},
        const std::function<void(float)>& progressCallback = nullptr
    );
    
    /**
     * Scan a directory for RPF files and extract all shaders.
     */
    ShaderScanResult ScanAndExtractShaders(
        const std::filesystem::path& gameDir,
        const std::filesystem::path& outputDir,
        const std::vector<uint8_t>& aesKey = {},
        const std::function<void(float)>& progressCallback = nullptr
    );
    
    /**
     * Check if SparkCLI tool is available (fallback method).
     */
    bool IsSparkCliAvailable();
    
    /**
     * Extract using SparkCLI (fallback for complex encrypted RPFs).
     * On non-Windows platforms, requires Mono.
     */
    ExtractionResult ExtractWithSparkCli(
        const std::filesystem::path& rpfPath,
        const std::filesystem::path& outputDir,
        const std::filesystem::path& keyPath = {},
        const std::function<void(float)>& progressCallback = nullptr
    );
}
