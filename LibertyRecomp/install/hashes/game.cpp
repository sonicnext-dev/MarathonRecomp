// GTA IV Game Files - Simplified file list for initial testing
// Full hash validation can be added later with fshasher

#include <utility>

extern const uint64_t GameHashes[];
extern const std::pair<const char *, uint32_t> GameFiles[];
extern const size_t GameFilesSize;

// No hash validation for now - use 0 hashes per file
const uint64_t GameHashes[] = {
    0ULL  // Placeholder - not used when files have 0 hash count
};

// GTA IV Xbox 360 files - hash count set to 0 to skip validation
const std::pair<const char *, uint32_t> GameFiles[] = {
    { "default.xex", 0 },
    { "audio.rpf", 0 },
    { "common.rpf", 0 },
    { "xbox360.rpf", 0 },
};

const size_t GameFilesSize = std::size(GameFiles);
