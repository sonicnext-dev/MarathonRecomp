# GTA IV Shader Pipeline Documentation

This document describes the shader extraction, conversion, and caching pipeline implemented in Liberty Recompiled for GTA IV.

> **See also:** [Installation Architecture](INSTALLATION_ARCHITECTURE.md) for platform-specific install directories and the full installation flow.

## Overview

GTA IV for Xbox 360 uses RAGE engine shaders stored in a proprietary `.fxc` container format. These shaders are **pre-compiled at build time** using XenosRecomp and embedded directly into the binary for optimal runtime performance.

| Platform | Graphics API | Shader Format | Cache Location |
|----------|-------------|---------------|----------------|
| Windows | Direct3D 12 | DXIL | Embedded in binary (`g_compressedDxilCache`) |
| Linux | Vulkan | SPIR-V | Embedded in binary (`g_compressedSpirvCache`) |
| macOS | Metal | AIR | Embedded in binary (`g_compressedAirCache`) |

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    BUILD-TIME Shader Pipeline                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. Extract shaders ──► tools/rage_fxc_extractor/               │
│                              │                                  │
│                              ▼                                  │
│                    ┌─────────────────────────┐                  │
│                    │ Xbox 360 Shader Binary  │                  │
│                    │   (0x102A11XX magic)    │                  │
│                    └───────────┬─────────────┘                  │
│                                │                                │
│  2. Recompile shaders ◄────────┘                                │
│         │                                                       │
│         ▼                                                       │
│  ┌─────────────────┐    ┌─────────────────┐                    │
│  │   XenosRecomp   │───►│ HLSL conversion │                    │
│  └─────────────────┘    └────────┬────────┘                    │
│                                  │                              │
│                                  ▼                              │
│                    ┌─────────────────────────┐                  │
│                    │ Platform-native compile │                  │
│                    │  DXIL / SPIR-V / AIR    │                  │
│                    └───────────┬─────────────┘                  │
│                                │                                │
│  3. Embed in binary ◄──────────┘                                │
│         │                                                       │
│         ▼                                                       │
│  ┌──────────────────────────────────────────┐                  │
│  │ LibertyRecompLib/shader/shader_cache.cpp │                  │
│  │  ├── g_shaderCacheEntries[1132]          │                  │
│  │  ├── g_compressedDxilCache               │                  │
│  │  ├── g_compressedSpirvCache              │                  │
│  │  └── g_compressedAirCache (8MB → 29MB)   │                  │
│  └──────────────────────────────────────────┘                  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## RAGE FXC Format

### Container Structure

```
RAGE FXC File (.fxc):
├── Magic: "rgxa" (0x61786772)
├── Vertex Fragment Count (1 byte)
├── Vertex Fragments[]
│   ├── Variables Count
│   ├── Variables[] (type, index, name)
│   ├── Shader Size (2 bytes)
│   ├── Shader Size 2 (2 bytes)
│   └── Bytecode[] (contains Xbox 360 shader)
├── Pixel Fragment Count (1 byte)
├── Unknown (5 bytes)
├── Pixel Fragments[] (same structure as vertex)
└── Additional metadata
```

### Xbox 360 Shader Container

Embedded within each fragment's bytecode:

```
Xbox 360 Shader Container:
├── Flags (4 bytes, magic 0x102A11XX)
├── Virtual Size (4 bytes, big-endian)
├── Physical Size (4 bytes, big-endian)
├── Field C (4 bytes)
├── Constant Table Offset (4 bytes)
├── Definition Table Offset (4 bytes)
├── Shader Offset (4 bytes)
├── Field 1C (4 bytes, must be 0)
├── Field 20 (4 bytes, must be 0)
└── Shader microcode and metadata
```

## Components

### 1. RAGE FXC Extractor Tool

**Location:** `tools/rage_fxc_extractor/`

Standalone tool for batch extraction of shaders from RAGE FXC files.

```bash
# Extract single file
./rage_fxc_extractor <input.fxc> [output_dir]

# Scan for Xbox 360 shaders
./rage_fxc_extractor --scan <input.fxc>

# Batch process directory
./rage_fxc_extractor --batch <input_dir> <output_dir>
```

### 2. Shader Converter Module

**Location:** `LibertyRecomp/install/shader_converter.h/cpp`

Integrated into the installation flow, automatically converts shaders during game installation.

```cpp
// Key functions
ShaderPlatform ShaderConverter::detectPlatform();
bool ShaderConverter::convertShaders(gameDir, cacheDir, platform, journal, callback);
bool ShaderConverter::extractShadersFromFxc(fxcPath, extractedShaders, journal);
bool ShaderConverter::isCacheValid(cacheDirectory, gameDirectory);
```

### 3. XenosRecomp Integration

**Location:** `tools/XenosRecomp/`

Converts extracted Xbox 360 shader binaries to HLSL, then compiles to target formats.

```bash
# Generate shader cache from extracted shaders
./XenosRecomp <extracted_dir> <output.cpp> <shader_common.h>
```

## Installation Flow Integration

The shader conversion is integrated into `Installer::install()`:

```cpp
// In installer.cpp
bool Installer::install(...) {
    // 1. Copy DLC files
    for (const DLCSource &dlcSource : sources.dlc) {
        copyFiles(...);
    }
    
    // 2. Copy base game files
    copyFiles({ GameFiles, GameFilesSize }, ...);
    
    // 3. Convert shaders to platform-native format
    {
        ShaderConversionJournal shaderJournal;
        ShaderPlatform platform = ShaderConverter::detectPlatform();
        
        if (!ShaderConverter::isCacheValid(cacheDir, gameDir)) {
            ShaderConverter::convertShaders(gameDir, cacheDir, platform, 
                                            shaderJournal, progressCallback);
        }
    }
    
    return true;
}
```

## Platform Detection

```cpp
ShaderPlatform ShaderConverter::detectPlatform() {
#if defined(_WIN32)
    return ShaderPlatform::DXIL;      // Windows - Direct3D 12
#elif defined(__APPLE__)
    return ShaderPlatform::AIR;       // macOS - Metal
#else
    return ShaderPlatform::SPIRV;     // Linux - Vulkan
#endif
}
```

## Cache Structure

Shaders are embedded in the binary at build time:

```
LibertyRecompLib/shader/
├── shader_cache.h          # ShaderCacheEntry struct definition
└── shader_cache.cpp        # Generated by XenosRecomp
    ├── g_shaderCacheEntries[1132]     # Shader metadata (hash, offsets, sizes)
    ├── g_compressedDxilCache          # DXIL bytecode (Windows)
    ├── g_compressedSpirvCache         # SPIR-V bytecode (Linux)
    └── g_compressedAirCache           # AIR bytecode (macOS, 8MB compressed)
```

## Runtime Usage

At runtime, `video.cpp` looks up shaders by hash from the embedded cache:

```cpp
// In video.cpp - FindShaderCacheEntry()
static ShaderCacheEntry* FindShaderCacheEntry(XXH64_hash_t hash)
{
    auto end = g_shaderCacheEntries + g_shaderCacheEntryCount;
    auto findResult = std::lower_bound(g_shaderCacheEntries, end, hash, 
        [](ShaderCacheEntry& lhs, XXH64_hash_t rhs) {
            return lhs.hash < rhs;
        });
    return findResult != end && findResult->hash == hash ? findResult : nullptr;
}

// In CreateShader()
auto findResult = FindShaderCacheEntry(hash);
if (findResult == nullptr) {
    // Shader not in cache - fatal error (no runtime fallback)
    LOG_ERROR("Shader not found: {:x}", hash);
    std::_Exit(1);
}
// Use pre-compiled shader from embedded cache
```

> **Note:** There is no runtime fallback. All shaders must be pre-compiled at build time.

## Statistics

From the 79 `.fxc` files in GTA IV:

| Metric | Value |
|--------|-------|
| Total .fxc files | 79 |
| Total extracted shaders | 1132 |
| Vertex shaders | ~600 |
| Pixel shaders | ~530 |
| AIR cache (macOS) | 8 MB compressed → 29 MB decompressed |
| SPIR-V cache (Linux) | 703 KB compressed → 5.5 MB decompressed |

## Error Handling

Shader lookup failures are **fatal** at runtime:
- Missing shader: `std::_Exit(1)` - game cannot continue
- All shaders must be pre-compiled at build time via XenosRecomp

Build-time extraction failures are non-fatal:
- Missing shader directory: Logged, build continues
- Parse failures: Individual file skipped, others processed

## Building the Tools

### RAGE FXC Extractor

```bash
cd tools/rage_fxc_extractor
mkdir build && cd build
cmake ..
make
```

### XenosRecomp

```bash
cd build_xenosrecomp
cmake ../tools/XenosRecomp
make
```

## Dependencies

- **xxHash** - For shader hash computation (XXH3_64bits)
- **fmt** - For string formatting
- **zstd** - For cache compression
- **DXC** - DirectX Shader Compiler (for HLSL to DXIL/SPIR-V)
- **metal-cpp** - For AIR compilation on macOS

## References

- [XenosRecomp README](../tools/XenosRecomp/README.md)
- [RAGE Shader Editor](https://github.com/ImpossibleEchoes/rage-shader-editor-cpp)
- [Xenia GPU Documentation](https://github.com/xenia-project/xenia/blob/master/docs/gpu.md)
