<p align="center">
    <img src="https://raw.githubusercontent.com/IsaacMarovitz/MarathonRecompResources/refs/heads/main/images/logo/Logo.png" width="512"/>
</p>

---

> [!CAUTION]
> This recompilation is still under active development and is NOT meant for public use. Support will not be provided until an official release.

Marathon Recompiled is an unofficial PC port of the Xbox 360 version of Sonic the Hedgehog (2006) created through the process of static recompilation. The port offers Windows, Linux, and macOS support.

**This project does not include any game assets. You must provide the files from your own legally acquired copy of the game to install or build Marathon Recompiled.**

[XenonRecomp](https://github.com/sonicnext-dev/XenonRecomp) and [XenosRecomp](https://github.com/sonicnext-dev/XenosRecomp) are the main recompilers used for converting the game's original PowerPC code and Xenos shaders into compatible C++ and HLSL code respectively. The development of these recompilers was directly inspired by [N64: Recompiled](https://github.com/N64Recomp/N64Recomp), which was used to create [Zelda 64: Recompiled](https://github.com/Zelda64Recomp/Zelda64Recomp).

## Table of Contents

- [Known Issues](#known-issues)
- [FAQ](#faq)
- [Building](#building)
- [Credits](#credits)

## Known Issues

Before reporting any issues, check if they are listed [here](https://github.com/sonicnext-dev/MarathonRecomp/issues).

### Original Game Bugs

Game bugs present on the original hardware are intentionally preserved and will not be fixed apart from a few minor exceptions in [#44](https://github.com/sonicnext-dev/MarathonRecomp/issues/44). Please do not report issues for these bugs and verify that the issue does not occur on original hardware before reporting. Bug reports for issues found in the original game will be rejected. Bugs that only happen in Marathon Recompiled must be accompanied by footage captured on original Xbox 360 hardware showing that the bug does not happen there.

### File Picker Unavailable on Steam Deck in Game Mode

Due to some restrictions of how the desktop environment on the Steam Deck works whilst in Game Mode, please note that you may need to at least first boot into Desktop Mode to be able to use the file picker to provide the game files.

Simply booting at least once in Desktop Mode will enable the Deck to use the file picker when going back to Game Mode. You can complete the entire installation process while in Desktop Mode to save yourself the trouble of browsing through Game Mode if necessary.

## FAQ

### Do you have a website?

Marathon Recompiled does not have an official website.

**Please link here when directing anyone to the project.**

> [!CAUTION]
> Do not download builds of Marathon Recompiled from anywhere but our [Releases](https://github.com/sonicnext-dev/MarathonRecomp/releases/latest) page.
>
> **We will never distribute builds on other websites, via Discord servers or via third-party update tools.**

### Why does the installer say my files are invalid?

The installer may display this error for several reasons. Please check the following to ensure your files are valid:

- Please read the [How to Install](#how-to-install) section and make sure you've acquired all of the necessary files correctly.

- Verify that you're not trying to add compressed files such as `.zip`, `.7z`, `.rar` or other formats.

- Only use the **Add Folder** option if you're sure you have a directory with the content's files already extracted, which means it'll only contain files like `.xex`, `.ar.00`, `.arl` and others. **This option will not scan your folder for compatible content**.

- Ensure that the files you've acquired correspond to the same region. **Discs and Title Updates from different regions can't be used together** and will fail to generate a patch.

- The installer will only accept **original and unmodified files**. Do not attempt to provide modified files to the installer.

### What are the keyboard bindings?

Pad|Key
-|-
A (Cross)|S
B (Circle)|D
X (Square)|A
Y (Triangle)|W
D-Pad - Up|Unbound
D-Pad - Down|Unbound
D-Pad - Left|Q
D-Pad - Right|E
Start|Return
Back (Select)|Backspace
Left Trigger (L2)|1
Right Trigger (R2)|3
Left Bumper (L1)|Unbound
Right Bumper (R1)|Unbound
Left Stick - Up|Up Arrow
Left Stick - Down|Down Arrow
Left Stick - Left|Left Arrow
Left Stick - Right|Right Arrow
Right Stick - Up|Unbound
Right Stick - Down|Unbound
Right Stick - Left|Unbound
Right Stick - Right|Unbound

---

You can change the keyboard bindings by editing `config.toml` located in the [configuration directory](#where-is-the-save-data-and-configuration-file-stored), although using a controller is highly recommended until Action Remapping is added in a future update.

Refer to the left column of [this enum template](https://github.com/sonicnext-dev/MarathonRecomp/blob/main/MarathonRecomp/user/config.cpp#L40) for a list of valid keys.

*The default keyboard layout is based on Devil's Details' keyboard layout for Sonic Generations (2011)*.

### Where is the save data and configuration file stored?

The save data and configuration files are stored at the following locations:

- Windows: `%APPDATA%\MarathonRecomp\`
- Linux: `~/.config/MarathonRecomp/`
- macOS: `~/Library/Application Support/MarathonRecomp/`

You will find the save data under the `save` folder. The configuration file is named `config.toml`.

### I want to update the game. How can I avoid losing my save data? Do I need to reinstall the game?

Updating the game can be done by simply copying and replacing the files from a [release](https://github.com/sonicnext-dev/MarathonRecomp/releases) on top of your existing installation. **Your save data and configuration will not be lost.** You won't need to reinstall the game, as the game files will always remain the same across versions of Marathon Recompiled.

### How can I force the game to store the save data and configuration in the installation folder?

You can make the game ignore the [default configuration paths](#where-is-the-save-data-and-configuration-file-stored) and force it to save everything in the installation directory by creating an empty `portable.txt` file. You are directly responsible for the safekeeping of your save data and configuration if you choose this option.

### How can I force the game to run the installation again?

While it's unlikely you'll need to do this unless you've modified your game files by accident, you can force the installer to run again by using the launch argument: `--install`.

### How can I force the game to run under X11 or Wayland?

Use either of the following arguments to force SDL to run under the video driver you want:

- X11: `--sdl-video-driver x11`
- Wayland: `--sdl-video-driver wayland`

The second argument will be passed directly to SDL as a hint to try to initialize the game with your preferred option.

### Where is the game data for the Flatpak version installed?

Given it is not possible to run the game where the Flatpak is stored, the game data will be installed to `~/.var/app/io.github.sonicnext_dev.marathonrecomp/data`. The Flatpak build will only recognize this directory as valid. Feel free to reuse this data directory with a native Linux build if you wish to switch in the future.

If you wish to move this data to another location, you can do so by creating a symlink from this directory to the one where you'll migrate your installation to.

> [!WARNING]
> Using external frame rate limiters or performance overlays may degrade performance or have negative consequences.

### Can I install the game with a PlayStation 3 copy?

**You cannot use the files from the PlayStation 3 version of the game.** Supporting these files would require an entirely new recompilation, as they have proprietary formatting that only works on PS3 and the code for these formats is only present in that version. All significant differences present in the PS3 version of the game have been included in this project as options.

### Why is the game detecting my PlayStation controller as an Xbox controller?

If you're using a third-party input translation layer (such as DS4Windows or Steam Input), it is recommended that you disable these for full controller support.

### What other platforms will be supported?

This project does not plan to support any more platforms other than Windows, Linux and macOS at the moment. Any contributors who wish to support more platforms should do so through a fork.

## Building

[Check out the building instructions here](/docs/BUILDING.md).

## Credits

### Marathon Recompiled
- [ga2mer](https://github.com/ga2mer): Creator of the recompilation, laying the initial foundation for the project. Other responsibilities include maintaining the audio backend and providing various patches for the game.

- [IsaacMarovitz](https://github.com/IsaacMarovitz): Graphics Programmer for the recompilation. Other responsibilities include maintaining macOS support and aiding in porting mod manager patches to the recompilation.

- [squidbus](https://github.com/squidbus): Graphics Programmer for the recompilation. Aided in researching the game's internals.

- [Hyper](https://github.com/hyperbx): Developer of system level features, such as achievement support and the custom menus, alongside various other patches and features to make the game feel right at home on modern systems. Aided in researching the game's internals.

- [Rei-san](https://github.com/ReimousTH): Developer of quality of life patches and extensive amounts of research into the game's internals.

- [Desko](https://github.com/FateForWindows): Aided in researching the game's internals and created many quality of life patches for the original game used by the recompilation.

- [LJSTAR](https://github.com/LJSTARbird): Artist behind the project logo. Provided French localization for the custom menus.

- [brianuuuSonic](https://github.com/brianuuu): Provided Japanese localization for the custom menus.

- [Kitzuku](https://github.com/Kitzuku): Provided German localization for the custom menus.

- [DaGuAr](https://x.com/TheDaguar): Provided Spanish localization for the custom menus.

- [NextinHKRY](https://github.com/NextinMono): Provided Italian localization for the custom menus.

- [Hotline Sehwani](https://www.youtube.com/channel/UC9NBX5UPq4fYvbr7bzvRvOg) & SilverIceSound: Produced the [installer music](https://www.youtube.com/watch?v=8mfOSTcTQNs) ([original prod.](https://www.youtube.com/watch?v=k_mGNwrxR5M) by [Tomoya Ohtani](https://www.youtube.com/@TomoyaOhtaniChannel)).

### Special Thanks
- Unleashed Recompiled Development Team: Created much of the ground work that made all of this possible and sped up development time considerably.

- [Skyth](https://github.com/blueskythlikesclouds): Provided graphics consultation and support for dynamic aspect ratio.

- [Darío](https://github.com/DarioSamo): Creator of the graphics hardware abstraction layer [plume](https://github.com/renderbag/plume), used by the project's graphics backend.

- [Syko](https://x.com/UltraSyko): Aided in identifying fonts used by the original SonicNext logo.

- Ray Vassos: Aided with German localization for the custom menus.

- [ocornut](https://github.com/ocornut): Creator of [Dear ImGui](https://github.com/ocornut/imgui), which is used as the backbone of the custom menus.

- Raymond Chen: Useful resources on Windows application development with his blog ["The Old New Thing"](https://devblogs.microsoft.com/oldnewthing/).
