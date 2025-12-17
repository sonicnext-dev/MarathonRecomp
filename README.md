<p align="center">
    <h1 align="center">Liberty Recompiled</h1>
    <p align="center"><em>GTA IV Xbox 360 Static Recompilation Project</em></p>
</p>

---

> [!CAUTION]
> This recompilation is in early development and is NOT meant for public use. This is a work-in-progress fork based on the MarathonRecomp framework.

Liberty Recompiled is an unofficial PC port of the Xbox 360 version of Grand Theft Auto IV created through the process of static recompilation. The port aims to offer Windows, Linux, and macOS support.

**This project does not include any game assets. You must provide the files from your own legally acquired copy of the game to install or build Liberty Recompiled.**

[XenonRecomp](https://github.com/sonicnext-dev/XenonRecomp) and [XenosRecomp](https://github.com/sonicnext-dev/XenosRecomp) are the main recompilers used for converting the game's original PowerPC code and Xenos shaders into compatible C++ and HLSL code respectively. The development of these recompilers was directly inspired by [N64: Recompiled](https://github.com/N64Recomp/N64Recomp), which was used to create [Zelda 64: Recompiled](https://github.com/Zelda64Recomp/Zelda64Recomp).

## Table of Contents

- [Project Status](#project-status)
- [Installation](#installation)
- [Building](#building)
- [Documentation](#documentation)
- [Credits](#credits)

## Project Status

This project is in **early development**. Current progress:

### Completed
- [x] XenonRecomp integration for PowerPC → C++ translation
- [x] XenosRecomp integration for Xenos → HLSL shader conversion
- [x] Cross-platform build system (Windows, Linux, macOS)
- [x] Installer wizard with ISO/folder/XContent support
- [x] Shader extraction pipeline (RAGE FXC → Xbox 360 → platform-native)
- [x] Platform-specific install directory support

### In Progress
- [ ] RAGE engine structure reverse engineering
- [ ] GPU/rendering pipeline implementation
- [ ] File system and RPF archive handling
- [ ] Game-specific patches and fixes

### TODO
- [ ] Audio system implementation
- [ ] Save data handling
- [ ] Input remapping for GTA IV controls
- [ ] Network/multiplayer stubs

## Installation

### Platform Install Directories

| Platform | Install Directory |
|----------|-------------------|
| Windows | `%LOCALAPPDATA%\LibertyRecomp\` |
| Linux | `~/.local/share/LibertyRecomp/` |
| macOS | `~/Library/Application Support/LibertyRecomp/` |

### Game Files Required

You need a legal copy of GTA IV for Xbox 360. Supported formats:
- Xbox 360 disc images (`.iso`)
- Extracted game folders
- XContent packages

See [Dumping Guide](/docs/DUMPING-en.md) for detailed extraction instructions.

## Building

[Check out the building instructions here](/docs/BUILDING.md).

### Quick Start

```bash
# Clone with submodules
git clone --recurse-submodules https://github.com/OZORDI/LibertyRecomp.git
cd LibertyRecomp

# Add game files to LibertyRecompLib/private/
# - default.xex
# - xbox360.rpf

# Configure and build (macOS example)
cmake . --preset macos-release
cmake --build ./out/build/macos-release --target LibertyRecomp
```

## Documentation

| Document | Description |
|----------|-------------|
| [Building Guide](/docs/BUILDING.md) | Build instructions for all platforms |
| [Dumping Guide](/docs/DUMPING-en.md) | How to extract game files from Xbox 360 |
| [Shader Pipeline](/docs/SHADER_PIPELINE.md) | RAGE FXC → platform shader conversion |
| [Installation Architecture](/docs/INSTALLATION_ARCHITECTURE.md) | Platform paths and install flow |

## Credits

### Liberty Recompiled
- Based on [MarathonRecomp](https://github.com/sonicnext-dev/MarathonRecomp) by the sonicnext-dev team

### Original Framework Credits
- [ga2mer](https://github.com/ga2mer): Creator of the original recompilation framework
- [IsaacMarovitz](https://github.com/IsaacMarovitz): Graphics Programmer
- [squidbus](https://github.com/squidbus): Graphics Programmer
- [Hyper](https://github.com/hyperbx): System level features developer
- And the entire MarathonRecomp team

### Special Thanks
- Unleashed Recompiled Development Team
- [Skyth](https://github.com/blueskythlikesclouds): Graphics consultation
- [Darío](https://github.com/DarioSamo): Creator of [plume](https://github.com/renderbag/plume) graphics abstraction layer
- [ocornut](https://github.com/ocornut): Creator of [Dear ImGui](https://github.com/ocornut/imgui)

### Developer Tools
- [SparkIV](https://github.com/ahmed605/SparkIV) by aru & Ahmed Walid (GPL-3.0) — vendored in this repo for RPF archive inspection
