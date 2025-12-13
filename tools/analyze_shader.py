#!/usr/bin/env python3
"""Analyze GTA IV shader files to understand their format."""

import os
import struct
import sys
from pathlib import Path

def analyze_file(filepath: str) -> None:
    """Analyze a single shader file."""
    with open(filepath, 'rb') as f:
        data = f.read()
    
    print(f"\n=== {os.path.basename(filepath)} ===")
    print(f"File size: {len(data)} bytes")
    
    if len(data) < 4:
        print("File too small")
        return
    
    # Check magic
    magic = struct.unpack('<I', data[0:4])[0]
    print(f"Magic (LE): 0x{magic:08X}")
    magic_be = struct.unpack('>I', data[0:4])[0]
    print(f"Magic (BE): 0x{magic_be:08X}")
    
    # Check for 'rgxa' magic (0x61786772)
    if magic == 0x61786772:
        print("Format: RAGE Xbox Archive (rgxa)")
    
    # Check if magic matches ASCII
    try:
        magic_ascii = data[0:4].decode('ascii')
        print(f"Magic as ASCII: '{magic_ascii}'")
    except:
        pass
    
    # Look for ShaderContainer signature (0x102A11XX)
    print("\nSearching for ShaderContainer (0x102A11XX)...")
    found_count = 0
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack('>I', data[i:i+4])[0]  # Try big-endian (Xbox 360)
        if (val & 0xFFFFFF00) == 0x102A1100:
            print(f"  Found at offset 0x{i:X}: flags=0x{val:08X}")
            found_count += 1
            if found_count >= 10:
                print("  ... (limiting to 10 matches)")
                break
    
    if found_count == 0:
        # Try little-endian
        for i in range(0, len(data) - 4, 4):
            val = struct.unpack('<I', data[i:i+4])[0]
            if (val & 0xFFFFFF00) == 0x102A1100:
                print(f"  Found (LE) at offset 0x{i:X}: flags=0x{val:08X}")
                found_count += 1
                if found_count >= 10:
                    break
    
    if found_count == 0:
        print("  No ShaderContainer signatures found")
    
    # Hex dump first 128 bytes
    print("\nFirst 128 bytes:")
    for i in range(0, min(128, len(data)), 16):
        hex_part = ' '.join(f'{b:02x}' for b in data[i:i+16])
        ascii_part = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data[i:i+16])
        print(f"  {i:04x}: {hex_part:<48} {ascii_part}")


def main():
    shader_dir = "/Users/Ozordi/Downloads/MarathonRecomp/RPF DUMP/Common RPF Dump/shaders/fxl_final"
    
    if not os.path.exists(shader_dir):
        print(f"Directory not found: {shader_dir}")
        return 1
    
    files = sorted(Path(shader_dir).glob("*.fxc"))[:5]  # Analyze first 5
    
    if not files:
        print("No .fxc files found")
        return 1
    
    print(f"Found {len(list(Path(shader_dir).glob('*.fxc')))} .fxc files, analyzing first 5...")
    
    for f in files:
        analyze_file(str(f))
    
    return 0


if __name__ == "__main__":
    sys.exit(main())
