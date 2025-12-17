# GTA IV Shader Pipeline Documentation

This document describes the shader extraction, conversion, and caching pipeline implemented in Liberty Recompiled for GTA IV.

> **See also:** [Installation Architecture](INSTALLATION_ARCHITECTURE.md) for platform-specific install directories and the full installation flow.

## Overview

GTA IV for Xbox 360 uses RAGE engine shaders stored in a proprietary `.fxc` container format. These shaders must be converted to platform-native formats for use on modern graphics APIs:

| Platform | Graphics API | Shader Format | Install Directory |
|----------|-------------|---------------|-------------------|
| Windows | Direct3D 12 | DXIL | `%LOCALAPPDATA%\LibertyRecomp\` |
| Linux | Vulkan | SPIR-V | `~/.local/share/LibertyRecomp/` |
| macOS | Metal | AIR | `~/Library/Application Support/LibertyRecomp/` |

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      Installation Flow                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. User selects ISO/folder ──► installer.cpp                   │
│                                     │                           │
│  2. Copy game files ◄───────────────┘                           │
│                                     │                           │
│  3. Shader conversion ◄─────────────┘                           │
│         │                                                       │
│         ▼                                                       │
│  ┌─────────────────┐    ┌─────────────────┐                    │
│  │ shader_converter │───►│ RAGE FXC Parser │                    │
│  │      .cpp        │    │                 │                    │
│  └─────────────────┘    └────────┬────────┘                    │
│                                  │                              │
│                                  ▼                              │
│                    ┌─────────────────────────┐                  │
│                    │ Xbox 360 Shader Binary  │                  │
│                    │   (0x102A11XX magic)    │                  │
│                    └───────────┬─────────────┘                  │
│                                │                                │
│                                ▼                                │
│                    ┌─────────────────────────┐                  │
│                    │     shader_cache/       │                  │
│                    │   ├── extracted/        │                  │
│                    │   │   └── *.bin         │                  │
│                    │   └── shader_cache.marker                  │
│                    └─────────────────────────┘                  │
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

After installation, shaders are cached at:

```
<install_dir>/
├── game/
│   └── shaders/
│       └── *.fxc (original RAGE FXC files)
└── shader_cache/
    ├── extracted/
    │   ├── gta_default/
    │   │   ├── gta_default_vs0.bin
    │   │   ├── gta_default_vs1.bin
    │   │   ├── gta_default_ps0.bin
    │   │   └── ...
    │   ├── gta_vehicle_paint1/
    │   │   └── ...
    │   └── ...
    └── shader_cache.marker
```

## Runtime Usage

At runtime, `video.cpp` loads shaders from the cache:

```cpp
// In video.cpp
const ShaderCacheEntry* entry = findShaderCacheEntry(hash);
if (entry) {
    // Use pre-compiled shader from cache
    loadCompiledShader(entry);
} else {
    // Fallback: runtime compilation (slower)
    compileShaderAtRuntime(shaderData);
}
```

## Statistics

From the 79 `.fxc` files in GTA IV:

| Metric | Value |
|--------|-------|
| Total .fxc files | 79 |
| Total extracted shaders | 1132 |
| Vertex shaders | ~600 |
| Pixel shaders | ~530 |
| Compressed cache size | ~31 MB |

## Error Handling

Shader conversion failures are **non-fatal**:
- Missing shader directory: Logged, installation continues
- Parse failures: Individual file skipped, others processed
- Compilation failures: Runtime fallback available

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
