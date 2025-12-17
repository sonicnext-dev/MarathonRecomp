#pragma once

#include <filesystem>
#include <functional>
#include <string>

/**
 * Xbox 360 ISO extraction using extract-xiso.
 * 
 * extract-xiso is a cross-platform tool for extracting Xbox/Xbox 360 ISOs.
 * Repository: https://github.com/XboxDev/extract-xiso
 * 
 * This wrapper handles:
 * - Detecting if source is an ISO file
 * - Locating or bundling extract-xiso binary
 * - Extracting ISO contents to target directory
 */
namespace IsoExtractor
{
    struct ExtractionResult
    {
        bool success = false;
        std::string errorMessage;
        std::filesystem::path extractedPath;
        uint64_t filesExtracted = 0;
    };
    
    /**
     * Check if a path points to an Xbox 360 ISO file.
     * Checks file extension and optionally validates magic bytes.
     */
    bool IsIsoFile(const std::filesystem::path& path);
    
    /**
     * Check if extract-xiso tool is available.
     * Looks in PATH and common installation locations.
     */
    bool IsToolAvailable();
    
    /**
     * Get the path to the extract-xiso executable.
     * Returns empty path if not found.
     */
    std::filesystem::path GetToolPath();
    
    /**
     * Extract an Xbox 360 ISO to the specified directory.
     * 
     * @param isoPath Path to the ISO file
     * @param outputDir Directory to extract contents to
     * @param progressCallback Optional callback for progress updates (0.0 - 1.0)
     * @return ExtractionResult with success status and any error messages
     */
    ExtractionResult Extract(
        const std::filesystem::path& isoPath,
        const std::filesystem::path& outputDir,
        const std::function<void(float)>& progressCallback = nullptr
    );
    
    /**
     * List contents of an ISO without extracting.
     * Useful for validation before extraction.
     */
    std::vector<std::string> ListContents(const std::filesystem::path& isoPath);
}
