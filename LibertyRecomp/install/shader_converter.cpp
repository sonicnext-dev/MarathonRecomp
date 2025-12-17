#include "shader_converter.h"

#include <fstream>
#include <cstring>
#include <algorithm>
#include <xxh3.h>
#include <fmt/format.h>

// RAGE FXC Magic: "rgxa" read as little-endian uint32
static constexpr uint32_t RAGE_FXC_MAGIC = 0x61786772;

// Xbox 360 shader container magic pattern
static constexpr uint32_t XBOX_SHADER_MAGIC_MASK = 0x00FFFFFF;
static constexpr uint32_t XBOX_SHADER_MAGIC = 0x00112A10;

struct RageFxcVariable
{
    uint8_t type;
    uint8_t index;
    uint16_t unknown;
    std::string name;
};

struct RageFxcFragment
{
    std::string name;
    uint8_t variablesCount;
    std::vector<RageFxcVariable> variables;
    uint16_t shaderSize;
    uint16_t shaderSize2;
    std::vector<uint8_t> bytecode;
};

class RageFxcParser
{
public:
    bool parse(const std::vector<uint8_t>& data, const std::string& filename)
    {
        m_data = data;
        m_pos = 0;
        m_filename = filename;
        
        // Check magic
        uint32_t magic = readU32();
        if (magic != RAGE_FXC_MAGIC)
        {
            return false;
        }
        
        // Read vertex fragment count
        uint8_t vertexFragmentCount = readU8();
        m_vertexFragments.resize(vertexFragmentCount);
        
        // Parse vertex fragments
        for (uint8_t i = 0; i < vertexFragmentCount; i++)
        {
            if (!parseFragment(m_vertexFragments[i]))
            {
                return false;
            }
        }
        
        // Read pixel fragment info
        uint8_t pixelFragmentCount = readU8();
        m_pos += 5; // Skip unknown bytes
        
        if (pixelFragmentCount > 0)
        {
            pixelFragmentCount--;
        }
        m_pixelFragments.resize(pixelFragmentCount);
        
        // Parse pixel fragments
        for (uint8_t i = 0; i < pixelFragmentCount; i++)
        {
            if (!parseFragment(m_pixelFragments[i]))
            {
                return false;
            }
        }
        
        return true;
    }
    
    std::vector<std::pair<std::string, std::vector<uint8_t>>> extractShaders()
    {
        std::vector<std::pair<std::string, std::vector<uint8_t>>> result;
        
        for (size_t i = 0; i < m_vertexFragments.size(); i++)
        {
            auto& frag = m_vertexFragments[i];
            size_t xboxOffset = findXboxShaderContainer(frag.bytecode);
            
            if (xboxOffset != std::string::npos)
            {
                std::string name = m_filename + "_vs" + std::to_string(i);
                std::vector<uint8_t> shaderData(
                    frag.bytecode.begin() + xboxOffset,
                    frag.bytecode.end()
                );
                result.emplace_back(name, std::move(shaderData));
            }
        }
        
        for (size_t i = 0; i < m_pixelFragments.size(); i++)
        {
            auto& frag = m_pixelFragments[i];
            size_t xboxOffset = findXboxShaderContainer(frag.bytecode);
            
            if (xboxOffset != std::string::npos)
            {
                std::string name = m_filename + "_ps" + std::to_string(i);
                std::vector<uint8_t> shaderData(
                    frag.bytecode.begin() + xboxOffset,
                    frag.bytecode.end()
                );
                result.emplace_back(name, std::move(shaderData));
            }
        }
        
        return result;
    }

private:
    std::vector<uint8_t> m_data;
    size_t m_pos = 0;
    std::string m_filename;
    std::vector<RageFxcFragment> m_vertexFragments;
    std::vector<RageFxcFragment> m_pixelFragments;
    
    uint8_t readU8()
    {
        if (m_pos >= m_data.size()) return 0;
        return m_data[m_pos++];
    }
    
    uint16_t readU16()
    {
        if (m_pos + 2 > m_data.size()) return 0;
        uint16_t value;
        memcpy(&value, &m_data[m_pos], 2);
        m_pos += 2;
        return value;
    }
    
    uint32_t readU32()
    {
        if (m_pos + 4 > m_data.size()) return 0;
        uint32_t value;
        memcpy(&value, &m_data[m_pos], 4);
        m_pos += 4;
        return value;
    }
    
    std::string readStringWithSize()
    {
        uint8_t len = readU8();
        if (m_pos + len > m_data.size()) return "";
        std::string str(reinterpret_cast<char*>(&m_data[m_pos]), len);
        m_pos += len;
        if (!str.empty() && str.back() == '\0')
        {
            str.pop_back();
        }
        return str;
    }
    
    bool parseFragment(RageFxcFragment& frag)
    {
        frag.variablesCount = readU8();
        frag.variables.resize(frag.variablesCount);
        
        for (uint8_t j = 0; j < frag.variablesCount; j++)
        {
            frag.variables[j].type = readU8();
            frag.variables[j].index = readU8();
            frag.variables[j].unknown = readU16();
            frag.variables[j].name = readStringWithSize();
        }
        
        frag.shaderSize = readU16();
        frag.shaderSize2 = readU16();
        
        if (m_pos + frag.shaderSize > m_data.size())
        {
            return false;
        }
        
        frag.bytecode.resize(frag.shaderSize);
        memcpy(frag.bytecode.data(), &m_data[m_pos], frag.shaderSize);
        m_pos += frag.shaderSize;
        
        return true;
    }
    
    size_t findXboxShaderContainer(const std::vector<uint8_t>& data)
    {
        for (size_t i = 0; i + 4 <= data.size(); i++)
        {
            uint32_t value;
            memcpy(&value, &data[i], 4);
            
            if ((value & XBOX_SHADER_MAGIC_MASK) == XBOX_SHADER_MAGIC)
            {
                return i;
            }
        }
        return std::string::npos;
    }
};

ShaderPlatform ShaderConverter::detectPlatform()
{
#if defined(_WIN32)
    return ShaderPlatform::DXIL;
#elif defined(__APPLE__)
    return ShaderPlatform::AIR;
#else
    return ShaderPlatform::SPIRV;
#endif
}

std::vector<std::filesystem::path> ShaderConverter::findShaderFiles(
    const std::filesystem::path& directory)
{
    std::vector<std::filesystem::path> result;
    
    if (!std::filesystem::exists(directory))
    {
        return result;
    }
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
    {
        if (entry.is_regular_file())
        {
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            
            if (ext == ".fxc")
            {
                result.push_back(entry.path());
            }
        }
    }
    
    return result;
}

bool ShaderConverter::extractShadersFromFxc(
    const std::filesystem::path& fxcPath,
    std::vector<std::pair<std::string, std::vector<uint8_t>>>& extractedShaders,
    ShaderConversionJournal& journal)
{
    // Read the FXC file
    std::ifstream file(fxcPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        journal.lastResult = ShaderConversionJournal::Result::ExtractionFailed;
        journal.lastErrorMessage = fmt::format("Failed to open shader file: {}", fxcPath.string());
        return false;
    }
    
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    file.close();
    
    // Parse the RAGE FXC format
    RageFxcParser parser;
    std::string filename = fxcPath.stem().string();
    
    if (!parser.parse(data, filename))
    {
        journal.lastResult = ShaderConversionJournal::Result::ExtractionFailed;
        journal.lastErrorMessage = fmt::format("Failed to parse shader file: {}", fxcPath.string());
        return false;
    }
    
    // Extract individual shaders
    extractedShaders = parser.extractShaders();
    
    return true;
}

bool ShaderConverter::isCacheValid(
    const std::filesystem::path& cacheDirectory,
    const std::filesystem::path& gameDirectory)
{
    std::filesystem::path cacheMarker = cacheDirectory / "shader_cache.marker";
    std::filesystem::path shaderDir = gameDirectory / "shaders";
    
    if (!std::filesystem::exists(cacheMarker))
    {
        return false;
    }
    
    // Check if cache is newer than shader directory
    auto cacheTime = std::filesystem::last_write_time(cacheMarker);
    
    for (const auto& shaderFile : findShaderFiles(shaderDir))
    {
        if (std::filesystem::last_write_time(shaderFile) > cacheTime)
        {
            return false;
        }
    }
    
    return true;
}

bool ShaderConverter::convertShaders(
    const std::filesystem::path& gameDirectory,
    const std::filesystem::path& cacheDirectory,
    ShaderPlatform platform,
    ShaderConversionJournal& journal,
    const std::function<bool()>& progressCallback)
{
    // Find shader directory in game installation
    std::filesystem::path shaderDir = gameDirectory / "shaders";
    
    if (!std::filesystem::exists(shaderDir))
    {
        // Try alternate locations
        shaderDir = gameDirectory / "common" / "shaders";
        
        if (!std::filesystem::exists(shaderDir))
        {
            // No shaders to convert - this is OK for some games
            journal.lastResult = ShaderConversionJournal::Result::NoShadersFound;
            journal.lastErrorMessage = "No shader directory found in game installation";
            return true; // Not a fatal error
        }
    }
    
    // Find all .fxc files
    auto shaderFiles = findShaderFiles(shaderDir);
    
    if (shaderFiles.empty())
    {
        journal.lastResult = ShaderConversionJournal::Result::NoShadersFound;
        journal.lastErrorMessage = "No .fxc shader files found";
        return true; // Not a fatal error
    }
    
    journal.shadersTotal = shaderFiles.size();
    
    // Create cache directory
    std::error_code ec;
    std::filesystem::create_directories(cacheDirectory, ec);
    if (ec)
    {
        journal.lastResult = ShaderConversionJournal::Result::CacheWriteFailed;
        journal.lastErrorMessage = fmt::format("Failed to create cache directory: {}", cacheDirectory.string());
        return false;
    }
    
    // Create subdirectory for extracted shaders
    std::filesystem::path extractedDir = cacheDirectory / "extracted";
    std::filesystem::create_directories(extractedDir, ec);
    
    // Process each shader file
    for (const auto& fxcPath : shaderFiles)
    {
        if (!progressCallback())
        {
            journal.lastResult = ShaderConversionJournal::Result::Cancelled;
            journal.lastErrorMessage = "Shader conversion was cancelled";
            return false;
        }
        
        std::vector<std::pair<std::string, std::vector<uint8_t>>> extractedShaders;
        
        if (!extractShadersFromFxc(fxcPath, extractedShaders, journal))
        {
            // Log warning but continue with other files
            journal.shadersProcessed++;
            continue;
        }
        
        // Save extracted shaders
        std::string fxcName = fxcPath.stem().string();
        std::filesystem::path shaderOutputDir = extractedDir / fxcName;
        std::filesystem::create_directories(shaderOutputDir, ec);
        
        for (const auto& [name, data] : extractedShaders)
        {
            std::filesystem::path outputPath = shaderOutputDir / (name + ".bin");
            std::ofstream out(outputPath, std::ios::binary);
            
            if (out.is_open())
            {
                out.write(reinterpret_cast<const char*>(data.data()), data.size());
                journal.bytesWritten += data.size();
            }
        }
        
        journal.shadersProcessed++;
    }
    
    // Create cache marker file with timestamp
    std::filesystem::path cacheMarker = cacheDirectory / "shader_cache.marker";
    std::ofstream marker(cacheMarker);
    if (marker.is_open())
    {
        marker << "GTA IV Shader Cache\n";
        marker << "Platform: ";
        switch (platform)
        {
            case ShaderPlatform::DXIL: marker << "DXIL (Windows/D3D12)\n"; break;
            case ShaderPlatform::SPIRV: marker << "SPIR-V (Vulkan)\n"; break;
            case ShaderPlatform::AIR: marker << "AIR (Metal)\n"; break;
            case ShaderPlatform::All: marker << "All\n"; break;
        }
        marker << "Shaders processed: " << journal.shadersProcessed << "\n";
        marker << "Bytes written: " << journal.bytesWritten << "\n";
    }
    
    journal.lastResult = ShaderConversionJournal::Result::Success;
    return true;
}
