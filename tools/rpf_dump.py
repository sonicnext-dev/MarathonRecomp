#!/usr/bin/env python3

import argparse
import io
import os
import struct
import sys
import zlib
from dataclasses import dataclass
from typing import List, Optional, Tuple


RPF2_MAGIC = 0x52504632  # 'RPF2'
RPF3_MAGIC = 0x52504633  # 'RPF3'


@dataclass
class RpfHeader:
    magic: int
    toc_size: int
    entry_count: int
    unknown: int
    encrypted: int


@dataclass
class RpfEntry:
    index: int
    is_dir: bool
    name: str
    # Directory fields
    content_index_raw: int = 0
    content_count_raw: int = 0
    # File fields
    size: int = 0
    offset: int = 0
    resource_type: int = 0
    flags: int = 0

    @property
    def is_compressed(self) -> bool:
        # For RPF2/3, the first bit is commonly described as "compressed".
        return (self.flags & 0x1) != 0


def _read_u32(f: io.BufferedReader) -> int:
    return struct.unpack('<I', f.read(4))[0]


def _read_i32(f: io.BufferedReader) -> int:
    return struct.unpack('<i', f.read(4))[0]


def _read_u24(f: io.BufferedReader) -> int:
    b = f.read(3)
    return b[0] | (b[1] << 8) | (b[2] << 16)


def read_header(f: io.BufferedReader) -> RpfHeader:
    magic = _read_u32(f)
    toc_size = _read_u32(f)
    entry_count = _read_u32(f)
    unknown = _read_u32(f)
    encrypted = _read_u32(f)
    return RpfHeader(magic, toc_size, entry_count, unknown, encrypted)


def _read_cstr_at(buf: bytes, offset: int) -> str:
    if offset < 0 or offset >= len(buf):
        return ""
    end = buf.find(b'\x00', offset)
    if end == -1:
        end = len(buf)
    raw = buf[offset:end]
    try:
        return raw.decode('utf-8', errors='replace')
    except Exception:
        return raw.decode('latin-1', errors='replace')


def _guess_name(buf: bytes, name_section_start: int, name_offset: int) -> str:
    # Heuristic: some docs say name_offset is absolute, others relative to name section.
    # Try absolute first, then relative.
    if 0 <= name_offset < len(buf):
        s = _read_cstr_at(buf, name_offset)
        if s:
            return s
    rel = name_section_start + name_offset
    if 0 <= rel < len(buf):
        s = _read_cstr_at(buf, rel)
        if s:
            return s
    return ""


def parse_entries(buf: bytes, header: RpfHeader) -> List[RpfEntry]:
    if header.encrypted != 0:
        raise RuntimeError(f"RPF is marked encrypted (flag={header.encrypted}). This tool only supports unencrypted RPF2/3 for now.")

    toc_start = 2048
    toc_end = toc_start + header.toc_size
    if toc_end > len(buf):
        raise RuntimeError("TOC extends past end of file")

    name_section_start = toc_end

    entries: List[RpfEntry] = []
    off = toc_start
    for i in range(header.entry_count):
        if off + 16 > toc_end:
            break

        name_offset = _read_i32(io.BufferedReader(io.BytesIO(buf[off:off + 4])))
        flags_or_size = struct.unpack('<I', buf[off + 4:off + 8])[0]
        third = buf[off + 8:off + 12]
        fourth = buf[off + 12:off + 16]

        # Directory vs file discriminator:
        # For RPF2, directory Flags commonly has bit 31 set (0x80000000).
        # For file entries, flags_or_size is the file size.
        is_dir = (flags_or_size & 0x80000000) != 0

        name = _guess_name(buf, name_section_start, name_offset)

        if is_dir:
            dir_flags = flags_or_size
            content_index = struct.unpack('<I', third)[0]
            content_count = struct.unpack('<I', fourth)[0]
            entries.append(RpfEntry(index=i, is_dir=True, name=name,
                                    content_index_raw=content_index,
                                    content_count_raw=content_count,
                                    flags=dir_flags))
        else:
            size = flags_or_size
            offset_u24 = third[0] | (third[1] << 8) | (third[2] << 16)
            resource_type = fourth[0]
            flags = struct.unpack('<I', fourth)[0]
            # Above flags read includes resource_type in low byte; keep both views.
            entries.append(RpfEntry(index=i, is_dir=False, name=name,
                                    size=size,
                                    offset=offset_u24,
                                    resource_type=resource_type,
                                    flags=flags))

        off += 16

    return entries


def build_paths(entries: List[RpfEntry]) -> List[str]:
    # RPF TOC is a flattened tree. Proper reconstruction requires interpreting
    # content_index/content_count encodings.
    # For now we just return entry names; later we can reconstruct full paths.
    return [e.name for e in entries]


def main() -> int:
    ap = argparse.ArgumentParser(description="List/extract entries from GTA IV RPF (RPF2/RPF3) archives (WIP).")
    ap.add_argument("rpf", help="Path to .rpf file")
    ap.add_argument("--list", action="store_true", help="List entries")
    ap.add_argument("--find", default=None, help="Case-insensitive substring filter for names")
    args = ap.parse_args()

    rpf_path = args.rpf
    with open(rpf_path, 'rb') as f:
        buf = f.read()

    header = read_header(io.BufferedReader(io.BytesIO(buf)))
    if header.magic not in (RPF2_MAGIC, RPF3_MAGIC):
        print(f"Unsupported RPF magic: 0x{header.magic:08X}")
        return 2

    print(f"RPF magic=0x{header.magic:08X} toc_size={header.toc_size} entry_count={header.entry_count} encrypted={header.encrypted}")

    try:
        entries = parse_entries(buf, header)
    except Exception as e:
        print(f"Failed to parse entries: {e}")
        return 3

    needle = args.find.lower() if args.find else None

    if args.list or args.find:
        for e in entries:
            if not e.name:
                continue
            if needle and needle not in e.name.lower():
                continue
            if e.is_dir:
                print(f"D {e.index:6d} flags=0x{e.flags:08X} name={e.name}")
            else:
                print(f"F {e.index:6d} size={e.size:9d} off=0x{e.offset:06X} type=0x{e.resource_type:02X} flags=0x{e.flags:08X} name={e.name}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
