#!/usr/bin/env python3
"""Analyze GTA IV shader files to understand their format."""

import struct
import sys
import os

def analyze_shader(path):
    with open(path, 'rb') as f:
        data = f.read()
    
    print(f"\n=== {os.path.basename(path)} ===")
    print(f"File size: {len(data)} bytes")
    print(f"First 64 bytes hex: {data[:64].hex()}")
    
    # Check for rgxa magic (0x61786772 = 'rgxa')
    if len(data) >= 4:
        magic = struct.unpack('<I', data[:4])[0]
        print(f"Magic (LE): 0x{magic:08X}", end="")
        if magic == 0x61786772:
            print(" = 'rgxa' (RAGE shader format)")
        else:
            magic_be = struct.unpack('>I', data[:4])[0]
            print(f" / Magic (BE): 0x{magic_be:08X}")
    
    # Look for ShaderContainer pattern 0x102A11xx (Xbox 360 format)
    print("\nScanning for ShaderContainer (0x102A11xx) patterns...")
    found = 0
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack('>I', data[i:i+4])[0]  # Big endian for Xbox
        if (val & 0xFFFFFF00) == 0x102A1100:
            print(f"  Found at offset 0x{i:04X}: flags=0x{val:08X}")
            found += 1
            if found >= 10:
                print("  ... (stopping after 10)")
                break
    
    if found == 0:
        print("  No ShaderContainer patterns found")
    
    return found

def main():
    shader_dir = "/Users/Ozordi/Downloads/MarathonRecomp/rpf dump/Common RPF Dump/shaders/db"
    
    if not os.path.exists(shader_dir):
        print(f"Directory not found: {shader_dir}")
        return 1
    
    files = [f for f in os.listdir(shader_dir) if f.endswith('.sps')]
    print(f"Found {len(files)} shader files")
    
    # Analyze first 3 files
    total_found = 0
    for f in files[:3]:
        path = os.path.join(shader_dir, f)
        total_found += analyze_shader(path)
    
    print(f"\n\nTotal ShaderContainer patterns found: {total_found}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
