#pragma once

#include <filesystem>
#include <string>

/**
 * Cross-platform path resolution for LibertyRecomp installation directories.
 * 
 * Platform-specific install directories:
 * - Windows: %LOCALAPPDATA%\LibertyRecomp\
 * - Linux:   ~/.local/share/LibertyRecomp/ (XDG compliant)
 * - macOS:   ~/Library/Application Support/LibertyRecomp/
 */
namespace PlatformPaths
{
    /**
     * Get the base installation directory for the current platform.
     * Creates the directory if it doesn't exist.
     */
    std::filesystem::path GetInstallDirectory();
    
    /**
     * Get the game files directory (where extracted game content goes).
     * Returns: <install_dir>/game/
     */
    std::filesystem::path GetGameDirectory();
    
    /**
     * Get the shader cache directory.
     * Returns: <install_dir>/shader_cache/
     */
    std::filesystem::path GetShaderCacheDirectory();
    
    /**
     * Get the saves directory.
     * Returns: <install_dir>/saves/
     */
    std::filesystem::path GetSavesDirectory();
    
    /**
     * Get temporary directory for extraction operations.
     * Returns: <install_dir>/temp/
     */
    std::filesystem::path GetTempDirectory();
    
    /**
     * Get the extracted RPF content directory.
     * Returns: <install_dir>/game/extracted/
     */
    std::filesystem::path GetExtractedRpfDirectory();
    
    /**
     * Get the path where the AES key should be stored/found.
     * Returns: <install_dir>/aes_key.bin
     */
    std::filesystem::path GetAesKeyPath();
    
    /**
     * Ensure all required directories exist.
     * Creates them if they don't.
     */
    void EnsureDirectoriesExist();
    
    /**
     * Clean up temporary files.
     */
    void CleanupTemp();
    
    /**
     * Get platform name as string ("Windows", "Linux", "macOS").
     */
    std::string GetPlatformName();
}
