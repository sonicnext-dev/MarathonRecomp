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
- [Building](#building)
- [Credits](#credits)

## Project Status

This project is in **early development**. The following components need to be implemented:

- [ ] RAGE engine structure reverse engineering
- [ ] GTA IV executable analysis and function mapping
- [ ] Shader recompilation for GTA IV's rendering pipeline
- [ ] Game-specific patches and fixes
- [ ] Save data handling
- [ ] Input remapping for GTA IV controls

## Prerequisites

To build this project, you will need:

- The GTA IV Xbox 360 `default.xex` executable
- GTA IV shader archives from the Xbox 360 version
- A legal copy of GTA IV for Xbox 360

## Building

[Check out the building instructions here](/docs/BUILDING.md).

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
