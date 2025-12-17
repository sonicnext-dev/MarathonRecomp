# Liberty Recompiled - Enhanced Installation Architecture

## Overview

This document describes the comprehensive installation flow that handles:
1. ISO to folder extraction (using extract-xiso)
2. RPF archive parsing and shader extraction
3. Platform-native shader cache generation
4. Cross-platform support (Windows, Linux, macOS)

---

## Platform-Specific Install Directories

### Windows
```
%LOCALAPPDATA%\LibertyRecomp\
├── game\                    # Extracted game files
│   ├── default.xex
│   ├── xbox360.rpf
│   └── extracted\           # RPF contents
│       └── common\shaders\*.fxc
├── shader_cache\            # Compiled DXIL shaders
├── saves\                   # Save data
└── temp\                    # Temporary extraction files
```

**Path:** `C:\Users\<username>\AppData\Local\LibertyRecomp\`

### Linux
```
~/.local/share/LibertyRecomp/
├── game/                    # Extracted game files
├── shader_cache/            # Compiled SPIR-V shaders
├── saves/
└── temp/
```

**Path:** `/home/<username>/.local/share/LibertyRecomp/`
**XDG compliant:** `$XDG_DATA_HOME/LibertyRecomp/`

### macOS
```
~/Library/Application Support/LibertyRecomp/
├── game/                    # Extracted game files
├── shader_cache/            # Compiled AIR shaders
├── saves/
└── temp/
```

**Path:** `/Users/<username>/Library/Application Support/LibertyRecomp/`

---

## Installation Flow

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          INSTALLATION WIZARD                            │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  1. SELECT SOURCE                                                       │
│     User selects:                                                       │
│     ├── ISO file (.iso)                                                │
│     ├── Extracted folder                                               │
│     └── XContent package                                               │
│                                                                         │
│  2. DETECT SOURCE TYPE                                                  │
│     ┌─────────────────┐                                                │
│     │ Is it an ISO?   │──YES──► Run extract-xiso                       │
│     └────────┬────────┘         └── Extract to temp directory          │
│              │NO                                                        │
│              ▼                                                          │
│     ┌─────────────────┐                                                │
│     │ Is it XContent? │──YES──► Use XContentFileSystem                 │
│     └────────┬────────┘                                                │
│              │NO                                                        │
│              ▼                                                          │
│     Use DirectoryFileSystem (already extracted)                        │
│                                                                         │
│  3. COPY GAME FILES                                                     │
│     Copy essential files to platform install directory:                │
│     ├── default.xex                                                    │
│     ├── xbox360.rpf                                                    │
│     ├── common.rpf                                                     │
│     └── other required files                                           │
│                                                                         │
│  4. SCAN FOR SHADERS                                                    │
│     Look for .fxc files in:                                            │
│     ├── Extracted game folders directly                                │
│     └── Inside RPF archives (extract if needed)                        │
│                                                                         │
│  5. EXTRACT & CONVERT SHADERS                                           │
│     For each .fxc file found:                                          │
│     ├── Parse RAGE FXC container (magic: 0x61786772 "rgxa")            │
│     ├── Extract Xbox 360 shader binaries (magic: 0x102A11XX)           │
│     └── Store in shader_cache/extracted/                               │
│                                                                         │
│  6. COMPILE SHADER CACHE                                                │
│     Detect platform and compile:                                       │
│     ├── Windows: HLSL → DXIL (DXC)                                     │
│     ├── Linux: HLSL → SPIR-V (DXC)                                     │
│     └── macOS: HLSL → AIR (Metal compiler)                             │
│                                                                         │
│  7. FINALIZE                                                            │
│     ├── Write shader_cache.marker                                      │
│     ├── Clean up temp files                                            │
│     └── Signal completion                                              │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Key Components

### Platform Paths (`install/platform_paths.h/cpp`)

Cross-platform directory resolution:

```cpp
#include "install/platform_paths.h"

// Get platform-specific install directory
auto installDir = PlatformPaths::GetInstallDirectory();
// Windows: %LOCALAPPDATA%\LibertyRecomp\
// Linux:   ~/.local/share/LibertyRecomp/
// macOS:   ~/Library/Application Support/LibertyRecomp/

auto gameDir = PlatformPaths::GetGameDirectory();        // <install>/game/
auto shaderDir = PlatformPaths::GetShaderCacheDirectory(); // <install>/shader_cache/
auto savesDir = PlatformPaths::GetSavesDirectory();       // <install>/saves/
```

### ISO Extractor (`install/iso_extractor.h/cpp`)

Xbox 360 ISO extraction using extract-xiso:

```cpp
#include "install/iso_extractor.h"

if (IsoExtractor::IsIsoFile(sourcePath))
{
    auto result = IsoExtractor::Extract(
        sourcePath,
        PlatformPaths::GetTempDirectory(),
        [](float progress) { /* update UI */ }
    );
    
    if (result.success)
    {
        // Use result.extractedPath as game source
    }
}
```

**Requirements:**
- `extract-xiso` must be installed and in PATH
- Repository: https://github.com/XboxDev/extract-xiso

### RPF Extractor (`install/rpf_extractor.h/cpp`)

RAGE Package File extraction:

```cpp
#include "install/rpf_extractor.h"

// Check if file has encrypted content
if (RpfExtractor::HasEncryptedFiles(rpfPath))
{
    // Load AES key (32 bytes for AES-256)
    std::vector<uint8_t> key;
    RpfExtractor::LoadAesKey("aes_key.bin", key);
}

// Scan game directory and extract all shaders
auto result = RpfExtractor::ScanAndExtractShaders(
    gameDir,
    outputDir,
    aesKey,
    [](float progress) { /* update UI */ }
);

printf("Found %zu shader files\n", result.fxcFiles.size());
```

---

## RPF2 Archive Format (GTA IV)

### Header (20 bytes, ALWAYS plaintext, LITTLE-ENDIAN)
```
Offset  Size  Field
0x00    4     Magic: 0x52504632 ("RPF2")
0x04    4     TOC Size (bytes)
0x08    4     Entry Count
0x0C    4     Unknown
0x10    4     Encryption Flag (0 = unencrypted, non-zero = encrypted files)
```

### Key Facts
- **Header is ALWAYS plaintext** - never encrypted
- **TOC is ALWAYS plaintext** - starts at offset 0x800 (2048 bytes)
- **Encryption is per-file** - only individual file data blocks are encrypted
- **Algorithm**: AES-256-ECB (no IV, no padding)
- **Compression**: zlib at block level

### Shader Location
```
common.rpf/
└── shaders/
    └── fxl_final/     ← 89 .fxc files
        ├── gta_default.fxc
        ├── gta_vehicle_paint1.fxc
        ├── gta_ped.fxc
        └── ...
```

---

## External Tools

### extract-xiso

Cross-platform Xbox 360 ISO extraction tool.

**Installation:**
```bash
# Build from source
git clone https://github.com/XboxDev/extract-xiso.git
cd extract-xiso
mkdir build && cd build
cmake ..
make
sudo make install

# macOS (Homebrew)
brew install extract-xiso
```

**Usage:**
```bash
extract-xiso -x game.iso -d /output/directory/
```

### SparkCLI (Fallback)

C# tool for RPF extraction, used as fallback for complex encrypted archives.

**Location:** `/SparkIV-master/SRC/SparkCLI/`

**Cross-platform usage:**
```bash
# Windows
SparkCLI.exe extract common.rpf ./output --key ./default.xex

# Linux/macOS (requires Mono)
mono SparkCLI.exe extract common.rpf ./output --key ./default.xex
```

---

## Shader Pipeline Integration

After extraction, shaders go through the conversion pipeline:

1. **RAGE FXC Parsing** - Extract Xbox 360 shader containers from `.fxc` files
2. **XenosRecomp** - Convert Xbox 360 Xenos shaders to HLSL
3. **Platform Compilation**:
   - Windows: DXC → DXIL
   - Linux: DXC → SPIR-V
   - macOS: Metal compiler → AIR

See [SHADER_PIPELINE.md](SHADER_PIPELINE.md) for detailed shader conversion documentation.

---

## Error Handling

### ISO Extraction Failures
- Check if `extract-xiso` is installed
- Verify ISO file is a valid Xbox 360 image
- Ensure sufficient disk space for extraction

### RPF Extraction Failures
- For encrypted RPFs, ensure AES key is available
- Try SparkCLI fallback for complex archives
- Pre-extract RPFs using OpenIV/SparkIV if automatic extraction fails

### Shader Conversion Failures
- Shader conversion is non-fatal
- Game can fall back to runtime compilation
- Check logs for specific shader errors

---

## References

- [extract-xiso GitHub](https://github.com/XboxDev/extract-xiso)
- [SparkIV/RageLib](https://github.com/ahmed605/SparkIV)
- [GTAMods RPF Wiki](https://gtamods.com/wiki/RPF_archive)
- [XenosRecomp README](../tools/XenosRecomp/README.md)
- [Shader Pipeline Documentation](SHADER_PIPELINE.md)
