/**
 * RAGE FXC Shader Extractor
 * 
 * Extracts Xbox 360 shader binaries from GTA IV RAGE FXC container format
 * for use with XenosRecomp shader translation.
 * 
 * Based on research from rage-shader-editor-cpp by ImpossibleEchoes
 */

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// RAGE FXC Magic: "rgxa" read as little-endian uint32
constexpr uint32_t RAGE_FXC_MAGIC = 0x61786772; // "rgxa" as bytes: 72 67 78 61

// Xbox 360 shader container magic pattern
constexpr uint32_t XBOX_SHADER_MAGIC_MASK = 0xFFFFFF00;
constexpr uint32_t XBOX_SHADER_MAGIC = 0x102A1100;

struct RageFxcHeader {
    uint32_t magic;              // "rgxa" = 0x61787267
    uint8_t  vertexFragmentCount;
    // Followed by variable-length data
};

struct ShaderVariable {
    uint8_t  type;
    uint8_t  index;
    uint16_t unknown;
    std::string name;
};

struct ShaderFragment {
    std::string name;
    uint8_t variablesCount;
    std::vector<ShaderVariable> variables;
    uint16_t shaderSize;
    uint16_t shaderSize2;
    std::vector<uint8_t> bytecode;
};

struct ExtractedShader {
    std::string name;
    std::string type; // "vs" or "ps"
    std::vector<uint8_t> bytecode;
    size_t xboxShaderOffset; // Offset to Xbox 360 shader container within bytecode
};

class RageFxcExtractor {
public:
    bool load(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filepath << std::endl;
            return false;
        }
        
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        m_data.resize(fileSize);
        file.read(reinterpret_cast<char*>(m_data.data()), fileSize);
        file.close();
        
        m_filename = fs::path(filepath).stem().string();
        m_pos = 0;
        
        return parse();
    }
    
    const std::vector<ExtractedShader>& getShaders() const {
        return m_extractedShaders;
    }
    
    bool extractToDirectory(const std::string& outputDir) {
        fs::create_directories(outputDir);
        
        for (const auto& shader : m_extractedShaders) {
            // Find Xbox 360 shader container within the bytecode
            size_t xboxOffset = findXboxShaderContainer(shader.bytecode);
            
            if (xboxOffset == std::string::npos) {
                std::cerr << "Warning: No Xbox 360 shader container found in " 
                          << shader.name << std::endl;
                // Still save the raw bytecode
                std::string filename = outputDir + "/" + shader.name + "_raw.bin";
                std::ofstream out(filename, std::ios::binary);
                out.write(reinterpret_cast<const char*>(shader.bytecode.data()), 
                         shader.bytecode.size());
                continue;
            }
            
            // Extract the Xbox 360 shader container
            std::string filename = outputDir + "/" + shader.name + ".bin";
            std::ofstream out(filename, std::ios::binary);
            out.write(reinterpret_cast<const char*>(shader.bytecode.data() + xboxOffset), 
                     shader.bytecode.size() - xboxOffset);
            
            std::cout << "Extracted: " << shader.name << " (" << shader.type 
                      << ", " << (shader.bytecode.size() - xboxOffset) << " bytes)" << std::endl;
        }
        
        return true;
    }
    
    // Scan for all Xbox 360 shader containers in the entire file
    std::vector<std::pair<size_t, size_t>> scanForXboxShaders() {
        std::vector<std::pair<size_t, size_t>> results;
        
        for (size_t i = 0; i + 4 <= m_data.size(); i++) {
            uint32_t value;
            memcpy(&value, &m_data[i], 4);
            
            // Check for Xbox 360 shader magic (big-endian in file)
            // The magic is 0x102A11XX where XX is flags
            if ((value & 0x00FFFFFF) == 0x00112A10) {
                // Found potential Xbox 360 shader container
                // Try to determine size from container header
                size_t containerSize = estimateContainerSize(i);
                results.push_back({i, containerSize});
            }
        }
        
        return results;
    }

private:
    std::vector<uint8_t> m_data;
    size_t m_pos;
    std::string m_filename;
    
    uint8_t m_vertexFragmentCount;
    uint8_t m_pixelFragmentCount;
    std::vector<ShaderFragment> m_vertexFragments;
    std::vector<ShaderFragment> m_pixelFragments;
    std::vector<ExtractedShader> m_extractedShaders;
    
    uint8_t readU8() {
        if (m_pos >= m_data.size()) return 0;
        return m_data[m_pos++];
    }
    
    uint16_t readU16() {
        if (m_pos + 2 > m_data.size()) return 0;
        uint16_t value;
        memcpy(&value, &m_data[m_pos], 2);
        m_pos += 2;
        return value;
    }
    
    uint32_t readU32() {
        if (m_pos + 4 > m_data.size()) return 0;
        uint32_t value;
        memcpy(&value, &m_data[m_pos], 4);
        m_pos += 4;
        return value;
    }
    
    std::string readStringWithSize() {
        uint8_t len = readU8();
        if (m_pos + len > m_data.size()) return "";
        std::string str(reinterpret_cast<char*>(&m_data[m_pos]), len);
        m_pos += len;
        // Remove null terminator if present
        if (!str.empty() && str.back() == '\0') {
            str.pop_back();
        }
        return str;
    }
    
    bool parse() {
        // Check magic
        uint32_t magic = readU32();
        if (magic != RAGE_FXC_MAGIC) {
            std::cerr << "Invalid RAGE FXC magic: 0x" << std::hex << magic << std::endl;
            return false;
        }
        
        // Read vertex fragment count
        m_vertexFragmentCount = readU8();
        m_vertexFragments.resize(m_vertexFragmentCount);
        
        // Parse vertex fragments
        for (uint8_t i = 0; i < m_vertexFragmentCount; i++) {
            if (!parseFragment(m_vertexFragments[i])) {
                std::cerr << "Failed to parse vertex fragment " << (int)i << std::endl;
                return false;
            }
        }
        
        // Read pixel fragment info (includes some unknown bytes)
        m_pixelFragmentCount = readU8();
        // Skip 5 unknown bytes that appear before pixel fragments
        m_pos += 5;
        
        // Adjust count (RAGE stores count+1 for some reason)
        if (m_pixelFragmentCount > 0) {
            m_pixelFragmentCount--;
        }
        m_pixelFragments.resize(m_pixelFragmentCount);
        
        // Parse pixel fragments
        for (uint8_t i = 0; i < m_pixelFragmentCount; i++) {
            if (!parseFragment(m_pixelFragments[i])) {
                std::cerr << "Failed to parse pixel fragment " << (int)i << std::endl;
                return false;
            }
        }
        
        // Build extracted shaders list
        for (size_t i = 0; i < m_vertexFragments.size(); i++) {
            ExtractedShader shader;
            shader.name = m_filename + "_vs" + std::to_string(i);
            shader.type = "vs";
            shader.bytecode = m_vertexFragments[i].bytecode;
            m_extractedShaders.push_back(std::move(shader));
        }
        
        for (size_t i = 0; i < m_pixelFragments.size(); i++) {
            ExtractedShader shader;
            shader.name = m_filename + "_ps" + std::to_string(i);
            shader.type = "ps";
            shader.bytecode = m_pixelFragments[i].bytecode;
            m_extractedShaders.push_back(std::move(shader));
        }
        
        return true;
    }
    
    bool parseFragment(ShaderFragment& frag) {
        frag.variablesCount = readU8();
        frag.variables.resize(frag.variablesCount);
        
        for (uint8_t j = 0; j < frag.variablesCount; j++) {
            frag.variables[j].type = readU8();
            frag.variables[j].index = readU8();
            frag.variables[j].unknown = readU16();
            frag.variables[j].name = readStringWithSize();
        }
        
        frag.shaderSize = readU16();
        frag.shaderSize2 = readU16();
        
        if (m_pos + frag.shaderSize > m_data.size()) {
            std::cerr << "Shader size exceeds file bounds" << std::endl;
            return false;
        }
        
        frag.bytecode.resize(frag.shaderSize);
        memcpy(frag.bytecode.data(), &m_data[m_pos], frag.shaderSize);
        m_pos += frag.shaderSize;
        
        return true;
    }
    
    size_t findXboxShaderContainer(const std::vector<uint8_t>& data) {
        for (size_t i = 0; i + 4 <= data.size(); i++) {
            uint32_t value;
            memcpy(&value, &data[i], 4);
            
            // Check for Xbox 360 shader magic (big-endian)
            // Pattern: 0x102A11XX where XX varies
            if ((value & 0x00FFFFFF) == 0x00112A10) {
                return i;
            }
        }
        return std::string::npos;
    }
    
    size_t estimateContainerSize(size_t offset) {
        // Xbox 360 shader container has size info in header
        // For now, return remaining size from offset
        if (offset + 24 > m_data.size()) {
            return m_data.size() - offset;
        }
        
        // Read virtual and physical sizes from container header
        uint32_t virtualSize, physicalSize;
        memcpy(&virtualSize, &m_data[offset + 4], 4);
        memcpy(&physicalSize, &m_data[offset + 8], 4);
        
        // Swap bytes (big-endian)
        virtualSize = __builtin_bswap32(virtualSize);
        physicalSize = __builtin_bswap32(physicalSize);
        
        return virtualSize + physicalSize;
    }
};

void printUsage(const char* programName) {
    std::cout << "RAGE FXC Shader Extractor for GTA IV\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " <input.fxc> [output_dir]\n";
    std::cout << "  " << programName << " --scan <input.fxc>\n";
    std::cout << "  " << programName << " --batch <input_dir> <output_dir>\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --scan    Scan file for Xbox 360 shader containers\n";
    std::cout << "  --batch   Process all .fxc files in directory\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string arg1 = argv[1];
    
    if (arg1 == "--scan" && argc >= 3) {
        // Scan mode: find all Xbox 360 shader containers in file
        RageFxcExtractor extractor;
        if (!extractor.load(argv[2])) {
            return 1;
        }
        
        auto containers = extractor.scanForXboxShaders();
        std::cout << "Found " << containers.size() << " Xbox 360 shader container(s):\n";
        for (const auto& [offset, size] : containers) {
            std::cout << "  Offset: 0x" << std::hex << offset 
                      << ", Size: " << std::dec << size << " bytes\n";
        }
        return 0;
    }
    
    if (arg1 == "--batch" && argc >= 4) {
        // Batch mode: process all .fxc files in directory
        std::string inputDir = argv[2];
        std::string outputDir = argv[3];
        
        int successCount = 0;
        int failCount = 0;
        
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (entry.path().extension() == ".fxc") {
                std::cout << "\nProcessing: " << entry.path().filename() << std::endl;
                
                RageFxcExtractor extractor;
                if (extractor.load(entry.path().string())) {
                    std::string shaderOutputDir = outputDir + "/" + 
                                                  entry.path().stem().string();
                    extractor.extractToDirectory(shaderOutputDir);
                    successCount++;
                } else {
                    failCount++;
                }
            }
        }
        
        std::cout << "\n=== Batch Complete ===\n";
        std::cout << "Success: " << successCount << "\n";
        std::cout << "Failed: " << failCount << "\n";
        return 0;
    }
    
    // Single file mode
    std::string inputFile = argv[1];
    std::string outputDir = argc >= 3 ? argv[2] : "./extracted_shaders";
    
    RageFxcExtractor extractor;
    if (!extractor.load(inputFile)) {
        return 1;
    }
    
    std::cout << "Loaded: " << inputFile << std::endl;
    std::cout << "Found " << extractor.getShaders().size() << " shader(s)\n";
    
    // Create output directory based on shader name
    std::string shaderName = fs::path(inputFile).stem().string();
    std::string shaderOutputDir = outputDir + "/" + shaderName;
    
    extractor.extractToDirectory(shaderOutputDir);
    
    return 0;
}
