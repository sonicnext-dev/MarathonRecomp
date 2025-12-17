#include "iso_extractor.h"

#include <array>
#include <cstdio>
#include <fstream>
#include <sstream>

#if defined(_WIN32)
#include <windows.h>
#define popen _popen
#define pclose _pclose
#endif

namespace IsoExtractor
{
    static std::string ExecuteCommand(const std::string& cmd, int& exitCode)
    {
        std::array<char, 128> buffer;
        std::string result;
        
#if defined(_WIN32)
        FILE* pipe = _popen(cmd.c_str(), "r");
#else
        FILE* pipe = popen(cmd.c_str(), "r");
#endif
        
        if (!pipe)
        {
            exitCode = -1;
            return "Failed to execute command";
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
        {
            result += buffer.data();
        }
        
#if defined(_WIN32)
        exitCode = _pclose(pipe);
#else
        exitCode = pclose(pipe);
#endif
        
        return result;
    }
    
    bool IsIsoFile(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
        {
            return false;
        }
        
        // Check extension
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext != ".iso")
        {
            return false;
        }
        
        // Optionally validate Xbox ISO magic (MICROSOFT*XBOX*MEDIA at offset 0x10000)
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            return false;
        }
        
        // Xbox 360 ISO has magic at offset 0x10000
        file.seekg(0x10000);
        if (!file)
        {
            return false;
        }
        
        char magic[20];
        file.read(magic, 20);
        if (!file)
        {
            // File too small, but might still be valid based on extension
            return true;
        }
        
        // Check for "MICROSOFT*XBOX*MEDIA"
        const char* expected = "MICROSOFT*XBOX*MEDIA";
        return std::memcmp(magic, expected, 20) == 0;
    }
    
    bool IsToolAvailable()
    {
        return !GetToolPath().empty();
    }
    
    std::filesystem::path GetToolPath()
    {
        // Common tool names
        const char* toolNames[] = {
            "extract-xiso",
#if defined(_WIN32)
            "extract-xiso.exe",
#endif
        };
        
        // Check if tool is in PATH
        for (const char* name : toolNames)
        {
            int exitCode = 0;
#if defined(_WIN32)
            std::string cmd = std::string("where ") + name + " 2>nul";
#else
            std::string cmd = std::string("which ") + name + " 2>/dev/null";
#endif
            std::string result = ExecuteCommand(cmd, exitCode);
            if (exitCode == 0 && !result.empty())
            {
                // Remove trailing newline
                while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
                {
                    result.pop_back();
                }
                return std::filesystem::path(result);
            }
        }
        
        // Check common installation paths
        std::vector<std::filesystem::path> commonPaths = {
#if defined(_WIN32)
            "C:\\Program Files\\extract-xiso\\extract-xiso.exe",
            "C:\\Tools\\extract-xiso.exe",
#elif defined(__APPLE__)
            "/usr/local/bin/extract-xiso",
            "/opt/homebrew/bin/extract-xiso",
#else
            "/usr/bin/extract-xiso",
            "/usr/local/bin/extract-xiso",
#endif
        };
        
        for (const auto& p : commonPaths)
        {
            if (std::filesystem::exists(p))
            {
                return p;
            }
        }
        
        return {};
    }
    
    ExtractionResult Extract(
        const std::filesystem::path& isoPath,
        const std::filesystem::path& outputDir,
        const std::function<void(float)>& progressCallback)
    {
        ExtractionResult result;
        
        // Check if ISO exists
        if (!std::filesystem::exists(isoPath))
        {
            result.errorMessage = "ISO file does not exist: " + isoPath.string();
            return result;
        }
        
        // Check if tool is available
        std::filesystem::path toolPath = GetToolPath();
        if (toolPath.empty())
        {
            result.errorMessage = "extract-xiso tool not found. Please install it from https://github.com/XboxDev/extract-xiso";
            return result;
        }
        
        // Create output directory
        std::error_code ec;
        std::filesystem::create_directories(outputDir, ec);
        if (ec)
        {
            result.errorMessage = "Failed to create output directory: " + ec.message();
            return result;
        }
        
        // Build command
        // extract-xiso -x <iso> -d <output_dir>
        std::ostringstream cmd;
        cmd << "\"" << toolPath.string() << "\" "
            << "-x \"" << isoPath.string() << "\" "
            << "-d \"" << outputDir.string() << "\" 2>&1";
        
        if (progressCallback)
        {
            progressCallback(0.0f);
        }
        
        // Execute
        int exitCode = 0;
        std::string output = ExecuteCommand(cmd.str(), exitCode);
        
        if (progressCallback)
        {
            progressCallback(1.0f);
        }
        
        if (exitCode != 0)
        {
            result.errorMessage = "extract-xiso failed with exit code " + std::to_string(exitCode) + ": " + output;
            return result;
        }
        
        // Count extracted files
        uint64_t fileCount = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(outputDir, ec))
        {
            if (entry.is_regular_file())
            {
                fileCount++;
            }
        }
        
        result.success = true;
        result.extractedPath = outputDir;
        result.filesExtracted = fileCount;
        
        return result;
    }
    
    std::vector<std::string> ListContents(const std::filesystem::path& isoPath)
    {
        std::vector<std::string> contents;
        
        std::filesystem::path toolPath = GetToolPath();
        if (toolPath.empty())
        {
            return contents;
        }
        
        // extract-xiso -l <iso>
        std::ostringstream cmd;
        cmd << "\"" << toolPath.string() << "\" -l \"" << isoPath.string() << "\" 2>&1";
        
        int exitCode = 0;
        std::string output = ExecuteCommand(cmd.str(), exitCode);
        
        if (exitCode == 0)
        {
            std::istringstream iss(output);
            std::string line;
            while (std::getline(iss, line))
            {
                if (!line.empty())
                {
                    contents.push_back(line);
                }
            }
        }
        
        return contents;
    }
}
