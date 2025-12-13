// find_save_restore.cpp
// Searches for register save/restore function addresses in a decompressed XEX
// Build: Compile with XenonUtils

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <file.h>
#include <image.h>

// Pattern definitions for register save/restore functions
struct Pattern {
    const char* name;
    uint8_t bytes[8];
    size_t size;
};

Pattern patterns[] = {
    { "restgprlr_14", {0xe9, 0xc1, 0xff, 0x68}, 4 },  // ld r14, -0x98(r1)
    { "savegprlr_14", {0xf9, 0xc1, 0xff, 0x68}, 4 },  // std r14, -0x98(r1)
    { "restfpr_14",   {0xc9, 0xcc, 0xff, 0x70}, 4 },  // lfd f14, -0x90(r12)
    { "savefpr_14",   {0xd9, 0xcc, 0xff, 0x70}, 4 },  // stfd f14, -0x90(r12)
    { "restvmx_14",   {0x39, 0x60, 0xfe, 0xe0, 0x7d, 0xcb, 0x60, 0xce}, 8 },
    { "savevmx_14",   {0x39, 0x60, 0xfe, 0xe0, 0x7d, 0xcb, 0x61, 0xce}, 8 },
    { "restvmx_64",   {0x39, 0x60, 0xfc, 0x00, 0x10, 0x0b, 0x60, 0xcb}, 8 },
    { "savevmx_64",   {0x39, 0x60, 0xfc, 0x00, 0x10, 0x0b, 0x61, 0xcb}, 8 },
};

void* searchPattern(const uint8_t* data, size_t dataSize, const uint8_t* pattern, size_t patternSize) {
    for (size_t i = 0; i <= dataSize - patternSize; i++) {
        if (memcmp(data + i, pattern, patternSize) == 0) {
            return (void*)(data + i);
        }
    }
    return nullptr;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: find_save_restore [XEX file path]\n");
        return 1;
    }

    printf("Loading XEX: %s\n", argv[1]);
    const auto file = LoadFile(argv[1]);
    if (file.empty()) {
        printf("Failed to load file\n");
        return 1;
    }

    printf("Parsing image...\n");
    auto image = Image::ParseImage(file.data(), file.size());
    
    printf("Image base: 0x%lx\n", image.base);
    printf("Number of sections: %zu\n", image.sections.size());

    for (const auto& section : image.sections) {
        printf("Section: base=0x%lx, size=0x%lx, flags=0x%x\n", 
               section.base, section.size, section.flags);

        for (const auto& pat : patterns) {
            uint8_t* data = section.data;
            size_t dataSize = section.size;
            uint8_t* found = (uint8_t*)searchPattern(data, dataSize, pat.bytes, pat.size);
            
            if (found) {
                size_t offset = found - data;
                size_t address = section.base + offset;
                printf("%s_address = 0x%08lX\n", pat.name, address);
            }
        }
    }

    printf("\nDone.\n");
    return 0;
}
