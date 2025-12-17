# Building Liberty Recompiled

Liberty Recompiled is an unofficial PC port of Grand Theft Auto IV for Xbox 360, created through static recompilation. This guide covers building the project from source.

## Supported Platforms

| Platform | Architecture | Status | CMake Preset |
|----------|--------------|--------|--------------|
| Windows | x64 | ✅ Supported | `x64-Clang-Release` |
| Windows | ARM64 | ✅ Supported | `arm64-Clang-Release` |
| Linux | x64 | ✅ Supported | `linux-release` |
| Linux | ARM64 | ✅ Supported | `linux-release` |
| macOS | ARM64 (Apple Silicon) | ✅ Supported | `macos-release` |
| macOS | x64 (Intel) | ✅ Supported | `macos-release` |

## 1. Clone the Repository

Clone **LibertyRecomp** with submodules using [Git](https://git-scm.com/).
```
git clone --recurse-submodules https://github.com/OZORDI/LibertyRecomp.git
```

### Windows
If you skipped the `--recurse-submodules` argument during cloning, you can run `update_submodules.bat` to ensure the submodules are pulled.

## 2. Add the Required Game Files

Copy the following files from your GTA IV Xbox 360 game and place them inside `./LibertyRecompLib/private/`:
- `default.xex` - Main executable (from game root)
- `xbox360.rpf` - Main game archive (from game root)

> [!TIP]
> It is recommended that you install the game using [an existing Liberty Recompiled release](https://github.com/OZORDI/LibertyRecomp/releases/latest) to acquire these files, otherwise you'll need to rely on third-party tools to extract them from your Xbox 360 disc or ISO.
>
> When sourcing these files from a Liberty Recompiled installation, they will be stored under the `game` subdirectory.

### Shader Files (Optional)

For shader development, you can also copy the shader files from `common/shaders/` to enable the shader pipeline:
- All `.fxc` files from the game's shader directories

These will be automatically processed during installation to generate platform-native shader caches.

## 3. Install Dependencies

### Windows (x64 and ARM64)
You will need to install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).

In the installer, you must select the following **Workloads** and **Individual components** for installation:
- Desktop development with C++
- C++ Clang Compiler for Windows
- C++ CMake tools for Windows

For **ARM64 builds**, also install:
- C++ ARM64/ARM64EC build tools (Latest)
- MSVC v143 - VS 2022 C++ ARM64/ARM64EC build tools

### Linux
The following command will install the required dependencies on a distro that uses `apt` (such as Debian-based distros).
```bash
sudo apt install autoconf automake libtool pkg-config curl cmake ninja-build clang clang-tools libgtk-3-dev
```
The following command will install the required dependencies on a distro that uses `pacman` (such as Arch-based distros).
```bash
sudo pacman -S base-devel ninja lld clang gtk3
```
You can also find the equivalent packages for your preferred distro.

> [!NOTE]
> This list may not be comprehensive for your particular distro and you may be required to install additional packages, should an error occur during configuration.

### macOS
You will need to install the latest Xcode from Apple.

The following commands will install additional required dependencies, depending on which package manager you use.

If you use Homebrew:
```bash
brew install cmake ninja pkg-config
```

If you use MacPorts:
```bash
sudo port install cmake ninja pkg-config
```

## 4. Build the Project

### Windows (x64)
1. Open the repository directory in Visual Studio and wait for CMake generation to complete. If you don't plan to debug, switch to the `Release` configuration.

> [!TIP]
> If you need a Release-performant build and want to iterate on development without debugging, **it is highly recommended** that you use the `RelWithDebInfo` configuration for faster compile times.

2. Under **Solution Explorer**, right-click and choose **Switch to CMake Targets View**.
3. Right-click the **LibertyRecomp** project and choose **Set as Startup Item**, then choose **Add Debug Configuration**.
4. Add a `currentDir` property to the first element under `configurations` in the generated JSON and set its value to the path to your game directory (where root is the directory containing `dlc`, `game`, etc).
5. Start **LibertyRecomp**. The initial compilation may take a while to complete due to code and shader recompilation.

#### Command Line Build (x64)
```powershell
# Open Developer Command Prompt for VS 2022, then:
cmake . --preset x64-Clang-Release
cmake --build .\out\build\x64-Clang-Release --target LibertyRecomp
```

### Windows (ARM64)
For ARM64 builds, use the command line:

```powershell
# Open ARM64 Developer Command Prompt for VS 2022, then:
cmake . --preset arm64-Clang-Release
cmake --build .\out\build\arm64-Clang-Release --target LibertyRecomp
```

> [!NOTE]
> The available Windows presets are:
> - **x64**: `x64-Clang-Debug`, `x64-Clang-RelWithDebInfo`, `x64-Clang-Release`
> - **ARM64**: `arm64-Clang-Debug`, `arm64-Clang-RelWithDebInfo`, `arm64-Clang-Release`

### Linux (x64 and ARM64)
The build process is the same for both x64 and ARM64 - the architecture is auto-detected.

1. Configure the project using CMake by navigating to the repository and running the following command.
```bash
cmake . --preset linux-release
```

> [!NOTE]
> The available presets are `linux-debug`, `linux-relwithdebinfo` and `linux-release`.

2. Build the project using the selected configuration.
```bash
cmake --build ./out/build/linux-release --target LibertyRecomp
```

3. Navigate to the directory that was specified as the output in the previous step and run the game.
```bash
./LibertyRecomp
```

#### Flatpak Build
For a sandboxed Flatpak build:
```bash
flatpak-builder --user --force-clean --install-deps-from=flathub builddir ./flatpak/io.github.ozordi.libertyrecomp.json
```

### macOS (ARM64 and x64)
The build process works for both Apple Silicon (ARM64) and Intel (x64) Macs.

1. Configure the project using CMake by navigating to the repository and running the following command.
```bash
# For Apple Silicon (ARM64) - default on M1/M2/M3 Macs
cmake . --preset macos-release -DCMAKE_OSX_ARCHITECTURES=arm64

# For Intel (x64) Macs
cmake . --preset macos-release -DCMAKE_OSX_ARCHITECTURES=x86_64
```

> [!NOTE]
> The available presets are `macos-debug`, `macos-relwithdebinfo` and `macos-release`.

2. Build the project using the selected configuration.
```bash
cmake --build ./out/build/macos-release --target LibertyRecomp
```

3. Navigate to the directory that was specified as the output in the previous step and run the game.
```bash
open "./out/build/macos-release/LibertyRecomp/Liberty Recompiled.app"
```

## 5. Shader Pipeline (Development)

Liberty Recompiled includes an automated shader pipeline that converts Xbox 360 RAGE engine shaders to platform-native formats during installation.

### How It Works

1. **During Installation**: The installer automatically extracts and converts shaders from `.fxc` files
2. **Platform Detection**: Automatically selects the correct format:
   - **Windows**: DXIL (Direct3D 12)
   - **Linux**: SPIR-V (Vulkan)
   - **macOS**: AIR (Metal)
3. **Caching**: Converted shaders are cached to avoid re-conversion on subsequent runs

### Building the Shader Tools

#### RAGE FXC Extractor (Standalone Tool)
```bash
cd tools/rage_fxc_extractor
mkdir build && cd build
cmake ..
make
```

#### XenosRecomp (Shader Compiler)
```bash
cd build_xenosrecomp
cmake ../tools/XenosRecomp
make
```

### Manual Shader Conversion

For development purposes, you can manually convert shaders:

```bash
# Extract shaders from RAGE FXC files
./tools/rage_fxc_extractor/build/rage_fxc_extractor --batch shader_batch/ extracted_shaders/

# Compile to shader cache
./build_xenosrecomp/XenosRecomp/XenosRecomp extracted_shaders/ LibertyRecompLib/shader/shader_cache.cpp tools/XenosRecomp/XenosRecomp/shader_common.h
```

For more details, see [SHADER_PIPELINE.md](SHADER_PIPELINE.md).

## 6. CI/CD Pipeline

Liberty Recompiled uses GitHub Actions for automated builds. On each tagged release, the pipeline builds for all supported platforms:

| Build Target | Runner | Output |
|--------------|--------|--------|
| Windows x64 | `windows-latest` | `LibertyRecomp-Windows-x64.zip` |
| Windows ARM64 | `windows-11-arm` | `LibertyRecomp-Windows-ARM64.zip` |
| Linux x64 | `ubuntu-24.04` | `LibertyRecomp-Linux-x64.tar.gz` |
| Linux ARM64 | `ubuntu-24.04-arm` | `LibertyRecomp-Linux-ARM64.tar.gz` |
| Linux Flatpak | `ubuntu-24.04` | `io.github.ozordi.libertyrecomp.flatpak` |
| macOS ARM64 | `macos-15` | `LibertyRecomp-macOS-ARM64.zip` |
| macOS x64 | `macos-13` | `LibertyRecomp-macOS-x64.zip` |

### Creating a Release

1. Tag the commit: `git tag v1.x.x`
2. Push the tag: `git push origin v1.x.x`
3. The release workflow will automatically build all targets and create a GitHub release

### Manual Workflow Dispatch

You can also trigger a release manually from the GitHub Actions tab using the "workflow_dispatch" option.

## 7. Project Structure

```
LibertyRecomp/
├── LibertyRecomp/          # Main application code
│   ├── apu/                # Audio processing
│   ├── cpu/                # CPU emulation / guest thread
│   ├── gpu/                # Graphics / video rendering
│   ├── hid/                # Human input devices
│   ├── install/            # Installer and shader converter
│   ├── kernel/             # Kernel imports / memory
│   ├── patches/            # GTA IV specific patches
│   ├── ui/                 # ImGui-based UI
│   └── user/               # User config and saves
├── LibertyRecompLib/       # Recompiled game code
│   ├── ppc/                # PowerPC recompiled functions
│   ├── shader/             # Shader cache
│   └── private/            # Game files (not in repo)
├── tools/                  # Development tools
│   ├── XenosRecomp/        # Xbox 360 shader recompiler
│   ├── rage_fxc_extractor/ # RAGE FXC shader extractor
│   └── bc_diff/            # Binary comparison tool
└── docs/                   # Documentation
```
