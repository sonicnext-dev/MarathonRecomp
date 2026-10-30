#pragma once

// Undefine this to generate a font atlas file in working directory.
// You also need to do this if you are testing localization, as only
// characters in the locale get added to the atlas.
#define ENABLE_IM_FONT_ATLAS_SNAPSHOT

struct GuestTexture;

enum class ImFontType
{
    Rodin,
    NewRodin,
    Arial
};

struct ImFontAtlasSnapshot
{
    std::vector<uint8_t> data;
    ankerl::unordered_dense::map<const void*, size_t> objects;
    std::vector<uint32_t> offsets;

    static inline bool IsInit{};
    static inline std::unique_ptr<GuestTexture> FontTexture;

    template<typename T1, typename T2>
    void SnapPointer(size_t offset, const T1& value, const T2& ptr, size_t count);

    template<typename T>
    void Traverse(size_t offset, const T& value);

    template<typename T>
    size_t Snap(const T& value);

    void Snap();

    static ImFontAtlas* Load();

    static void Init(bool in_isInstaller = false);

    // When ENABLE_IM_FONT_ATLAS_SNAPSHOT is undefined, this creates the font runtime instead. 
    static ImFont* GetFont(ImFontType in_fontType);
};
