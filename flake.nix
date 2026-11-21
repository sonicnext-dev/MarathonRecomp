# Nix dev shell for building Marathon Recompiled.
# You will need to have flakes enabled in Nix.
#
# Usage:
#   First enter dev shell:
#     nix develop
#
#   Generate `ninja` build files:
#     cmake -B out/build/nix -G Ninja -DCMAKE_BUILD_TYPE=Release
#
#   Build (Embrace Solaris):
#     cmake --build out/build/nix --target MarathonRecomp
#
#  DCMAKE_BUILD_TYPE can be Release, Debug or RelWithDebInfo
#  ofc; don't forget to copy files from ROM, per BUILDING.md
{
  description = "Marathon Recompiled dev shell";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};

    # Libraries loaded at runtime, i.e. by `ld`
    # If all you're familiar with is Windows, think of this as your LoadLibraryW targets.
    runtimeLibs = with pkgs; [
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
        llvmPackages.lld
        llvmPackages.llvm

        git
      ];
      buildInputs = with pkgs; [curl freetype gtk3] ++ runtimeLibs;

      # Disable unneeded security feature that ends up
      # as `xor`(s) at end of funcs.
      hardeningDisable = ["zerocallusedregs"];

      # Use LLD from LLVM from linking.
      NIX_CFLAGS_LINK = "-fuse-ld=lld";

      # Needs to be set for `CMakeLists.txt`.
      # For Nix, we vendor deps from nixpkgs; skipping vcpkg.
      VCPKG_ROOT = "/var/empty";

      # Tell `ld` about our runtime libs.
      LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath runtimeLibs;
    };
  };
}
