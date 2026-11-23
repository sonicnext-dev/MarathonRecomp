# Nix dev shell for building Marathon Recompiled.
# You will need to have flakes enabled in Nix.
#
# Suitable for development only. For build; is a bit hard; because of copyright-
# we can't really vendor files in `MarathonRecompLib/private`.
#
# ==========
#
# Usage:
#   First enter dev shell:
#     nix develop
#
#   Generate `ninja` build files:
#     cmake -B out/build/nix -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
#
#   Build:
#     cmake --build out/build/nix --target MarathonRecomp
#
#   Run (Embrace Solaris):
#     ./out/build/nix/MarathonRecomp/MarathonRecomp
#
#  DCMAKE_BUILD_TYPE can be Release, Debug or RelWithDebInfo
#  ofc; don't forget to copy files from ROM (-> MarathonRecompLib), per BUILDING.md
#
# ==========
#
#  For editing code on Nix:
#
#  1. Move compile_commands.json to root, to make clangd happy.
#     ln -sfn "$PWD/out/build/nix/compile_commands.json" compile_commands.json
#
#  2. `nix develop` before launching IDE, or use IDE plugin like `Niv Environment Selector`.
#     - Ctrl+Shift+P -> Nix-Env: Select Environment [for VSCode]
#
# ==========
#
# For non-NixOS (other-Linux or macOS with nix):
#   This is untested. Build should work, but you may need to use something like
#   `nixGL` for your host GPU drivers to be picked up.
#
#   `nix run --impure github:nix-community/nixGL -- out/build/nix/MarathonRecomp/MarathonRecomp`
{
  description = "Marathon Recompiled dev shell";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};

    # Libraries loaded at runtime, i.e. by `ld`
    # If all you're familiar with is Windows, think of this as your LoadLibraryW targets.
    #
    # Use `https://search.nixos.org/packages?channel=unstable` or `nixd` to find packages.
    runtimeLibs = with pkgs; [
      # Prebuilt `libdxcompiler.so` (in XenosRecomp) needs zlib.
      zlib # what year is this ????

      # Vulkan
      vulkan-loader

      # SDL3: X11
      libX11
      libXcursor
      libXext
      libXfixes
      libXi
      libXrandr
      libXScrnSaver
      libXtst

      # SDL3: Wayland
      wayland
      libdecor # Window decorations
      libxkbcommon # Keyboard

      # SDL3: audio
      alsa-lib # oldschool audio
      libpulseaudio # Pulse (2011+ distros)
      pipewire # Modern audio (2018+ distros)

      # SDL3: input
      udev
    ];
  in {
    devShells.${system}.default = pkgs.mkShell.override {stdenv = pkgs.clangStdenv;} {
      nativeBuildInputs = with pkgs; [
        # Build tools
        cmake
        ninja # ninja go brrr

        # Finding build deps
        pkg-config

        # Needed by SDL3 @ build time
        wayland-scanner

        # Linker for LTO (link-time optimization)
        llvmPackages.bintools # wrapped ld
        llvmPackages.llvm

        # clangd: IDE language server
        clang-tools

        git
      ];
      buildInputs = with pkgs; [curl freetype gtk3] ++ runtimeLibs;

      # Disable unneeded security feature that ends up
      # as `xor`(s) at end of funcs.
      hardeningDisable = ["zerocallusedregs"];

      # Use LLD from LLVM from linking.
      # It's a bit faster, so your builds go brr.
      NIX_CFLAGS_LINK = "-fuse-ld=lld";

      # Needs to be set for `CMakeLists.txt`.
      # For Nix, we vendor deps from nixpkgs; skipping vcpkg.
      VCPKG_ROOT = "/var/empty";

      # Tell `ld` about our runtime libs.
      LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath runtimeLibs;

      # clangStdenv's `clang` package puts an 'unwrapped' toolset
      # first on PATH. Its clangd can't find nix stdlib headers.
      # Meaning that IDE support is fucked.
      #
      # Prepend wrapped clangd so VSCode and friends pick that one.
      shellHook = ''
        export PATH="${pkgs.clang-tools}/bin:$PATH"
      '';
    };
  };
}
