#include "imgui_snapshot.h"
#include <kernel/xdbf.h>
#include <locale/locale.h>
#include <os/locale.h>
#include <res/font/im_font_atlas.bin.h>
#include <res/font/im_font_atlas.dds.h>
#include <user/config.h>
#include <app.h>
#include <decompressor.h>

extern ImFontBuilderIO g_fontBuilderIO;

struct FontInfo
{
    const char* pFileName{};
    std::vector<ImWchar> GlyphRanges{};
};

static std::map<ImFontType, FontInfo> g_fonts =
{
    { ImFontType::Rodin,    { "FOT-RodinPro-DB.otf" } },
    { ImFontType::NewRodin, { "FOT-NewRodinPro-UB.otf" } },
    { ImFontType::Arial,    { "ArialUni.ttf" } }
};

template<typename T1, typename T2>
void ImFontAtlasSnapshot::SnapPointer(size_t offset, const T1& value, const T2& ptr, size_t count)
{
    if (ptr != nullptr && count != 0)
    {
        if (!objects.contains(ptr))
        {
            constexpr size_t ALIGN = alignof(std::remove_pointer_t<T2>);
            constexpr size_t SIZE = sizeof(std::remove_pointer_t<T2>);

            size_t ptrOffset = (data.size() + ALIGN - 1) & ~(ALIGN - 1);
            data.resize(ptrOffset + SIZE * count);
            memcpy(&data[ptrOffset], ptr, SIZE * count);

            for (size_t i = 0; i < count; i++)
            {
                size_t curPtrOffset = ptrOffset + SIZE * i;
                objects[&ptr[i]] = curPtrOffset;
                Traverse(curPtrOffset, ptr[i]);
            }
        }

        size_t fieldOffset = offset + (reinterpret_cast<ptrdiff_t>(&ptr) - reinterpret_cast<ptrdiff_t>(&value));
        *reinterpret_cast<size_t*>(&data[fieldOffset]) = objects[ptr];
        offsets.push_back(fieldOffset);
    }
}

template<typename T>
void ImFontAtlasSnapshot::Traverse(size_t offset, const T& value)
{
    if constexpr (std::is_pointer_v<T>)
    {
        SnapPointer(offset, value, value, 1);
    }
    else if constexpr (std::is_same_v<T, ImFontAtlas>)
    {
        SnapPointer(offset, value, value.ConfigData.Data, value.ConfigData.Size);
        SnapPointer(offset, value, value.CustomRects.Data, value.CustomRects.Size);
        SnapPointer(offset, value, value.Fonts.Data, value.Fonts.Size);
    }
    else if constexpr (std::is_same_v<T, ImFont>)
    {
        SnapPointer(offset, value, value.IndexAdvanceX.Data, value.IndexAdvanceX.Size);
        SnapPointer(offset, value, value.IndexLookup.Data, value.IndexLookup.Size);
        SnapPointer(offset, value, value.Glyphs.Data, value.Glyphs.Size);
        SnapPointer(offset, value, value.FallbackGlyph, 1);
        SnapPointer(offset, value, value.ContainerAtlas, 1);
        SnapPointer(offset, value, value.ConfigData, value.ConfigDataCount);
    }
    else if constexpr (std::is_same_v<T, ImFontAtlasCustomRect>)
    {
        SnapPointer(offset, value, value.Font, 1);
    }  
    else if constexpr (std::is_same_v<T, ImFontConfig>)
    {
        SnapPointer(offset, value, value.GlyphRanges, value.GlyphRanges != nullptr ? wcslen(reinterpret_cast<const wchar_t*>(value.GlyphRanges)) + 1 : 0);
        SnapPointer(offset, value, value.DstFont, 1);
    }
}

template<typename T>
size_t ImFontAtlasSnapshot::Snap(const T& value)
{
    size_t offset = (data.size() + alignof(T) - 1) & ~(alignof(T) - 1);
    data.resize(offset + sizeof(T));
    memcpy(&data[offset], &value, sizeof(T));
    objects[&value] = offset;
    Traverse(offset, value);
    return offset;
}

struct ImFontAtlasSnapshotHeader
{
    uint32_t imguiVersion;
    uint32_t dataOffset;
    uint32_t offsetCount;
    uint32_t offsetsOffset;
};

void ImFontAtlasSnapshot::Snap()
{
    data.resize(sizeof(ImFontAtlasSnapshotHeader));
    size_t dataOffset = Snap(*ImGui::GetIO().Fonts);

    size_t offsetsOffset = data.size();
    std::sort(offsets.begin(), offsets.end());

    data.insert(data.end(), 
        reinterpret_cast<uint8_t*>(offsets.data()),
        reinterpret_cast<uint8_t*>(offsets.data() + offsets.size()));

    auto header = reinterpret_cast<ImFontAtlasSnapshotHeader*>(data.data());
    header->imguiVersion = IMGUI_VERSION_NUM;
    header->dataOffset = dataOffset;
    header->offsetCount = offsets.size();
    header->offsetsOffset = offsetsOffset;
}

static std::unique_ptr<uint8_t[]> g_imFontAtlas;

ImFontAtlas* ImFontAtlasSnapshot::Load()
{
    g_imFontAtlas = decompressZstd(g_im_font_atlas, g_im_font_atlas_uncompressed_size);

    auto header = reinterpret_cast<ImFontAtlasSnapshotHeader*>(g_imFontAtlas.get());
    assert(header->imguiVersion == IMGUI_VERSION_NUM && "ImGui version mismatch, the font atlas needs to be regenerated!");

    auto offsetTable = reinterpret_cast<uint32_t*>(g_imFontAtlas.get() + header->offsetsOffset);
    for (size_t i = 0; i < header->offsetCount; i++)
    {
        *reinterpret_cast<size_t*>(g_imFontAtlas.get() + (*offsetTable)) += reinterpret_cast<size_t>(g_imFontAtlas.get());
        ++offsetTable;
    }

    return reinterpret_cast<ImFontAtlas*>(g_imFontAtlas.get() + header->dataOffset);
}

static void GetGlyphs(std::set<ImWchar>& glyphs, const std::string_view& value)
{
    const char* cur = value.data();
    while (cur < value.data() + value.size())
    {
        unsigned int c;
        cur += ImTextCharFromUtf8(&c, cur, value.data() + value.size());
        glyphs.emplace(c);
    }
}

static void GenerateGlyphRanges(ImFontType in_fontType, bool in_isInstaller = false)
{
    std::vector<std::string_view> localeStrings{};

    for (auto& config : g_configDefinitions)
        config->GetLocaleStrings(localeStrings);

    std::set<ImWchar> glyphs;

    for (size_t i = 0x20; i <= 0xFF; i++)
        glyphs.emplace(i);

    for (auto& localeString : localeStrings)
        GetGlyphs(glyphs, localeString);

    for (auto& [name, locale] : g_locale)
    {
        for (auto& [language, value] : locale)
            GetGlyphs(glyphs, value);
    }

    auto& rFontInfo = g_fonts[in_fontType];

    if (in_isInstaller)
    {
        LOGFN_WARNING("Generating font atlas for \"{}\" in the installer wizard. Glyphs may be missing!", rFontInfo.pFileName);
    }
    else
    {
        if (in_fontType == ImFontType::Rodin)
        {
            for (size_t i = XDBF_LANGUAGE_ENGLISH; i <= XDBF_LANGUAGE_ITALIAN; i++)
            {
                auto achievements = g_xdbfWrapper.GetAchievements(static_cast<EXDBFLanguage>(i));

                for (auto& achievement : achievements)
                {
                    GetGlyphs(glyphs, achievement.Name);
                    GetGlyphs(glyphs, achievement.UnlockedDesc);
                    GetGlyphs(glyphs, achievement.LockedDesc);
                }
            }
        }
        else if (in_fontType == ImFontType::Arial)
        {
            const std::array<void*, 3> stageSelectStrings =
            {
                g_memory.Translate(0x820336C4),
                g_memory.Translate(0x820336C8),
                g_memory.Translate(0x820336D0)
            };

            for (const auto& rStr : stageSelectStrings)
                GetGlyphs(glyphs, os::locale::ConvertToUTF8(os::locale::Encoding::ShiftJIS, reinterpret_cast<const char*>(rStr)));

            if (const auto pResourceManager = Sonicteam::SoX::ResourceManager::GetInstance())
            {
                if (const auto pStageSelectLua = pResourceManager->LoadResource<Sonicteam::LuaSystem, Sonicteam::LuaSystemManager>("scripts/stage/stageselect.lua"))
                {
                    if (const auto pStageListTable = pStageSelectLua->GetTable("StageList"))
                    {
                        if (pStageListTable->Iterate() && pStageListTable->IsTable())
                        {
                            do
                            {
                                GetGlyphs(glyphs, os::locale::ConvertToUTF8(os::locale::Encoding::ShiftJIS, pStageListTable->GetElement("Comment")));
                            }
                            while (pStageListTable->Iterate());
                        }
                    }
                }
            }
        }
    }

    auto& rGlyphRanges = rFontInfo.GlyphRanges;

    for (auto glyph : glyphs)
    {
        if (rGlyphRanges.empty() || (rGlyphRanges.back() + 1) != glyph)
        {
            rGlyphRanges.push_back(glyph);
            rGlyphRanges.push_back(glyph);
        }
        else
        {
            rGlyphRanges.back() = glyph;
        }
    }

    rGlyphRanges.push_back(0);

    ImGui::GetIO().Fonts->AddFontFromFileTTF(rFontInfo.pFileName, 24.0f, nullptr, rGlyphRanges.data());
}

void ImFontAtlasSnapshot::Init(bool in_isInstaller)
{
    if (IsInitialised)
        return;

    auto& io = ImGui::GetIO();

#ifdef ENABLE_IM_FONT_ATLAS_SNAPSHOT
    IM_DELETE(io.Fonts);
    io.Fonts = ImFontAtlasSnapshot::Load();

    FontTexture = LoadTexture
    (
        decompressZstd
        (
            g_im_font_atlas_texture,
            g_im_font_atlas_texture_uncompressed_size
        )
        .get(),

        g_im_font_atlas_texture_uncompressed_size
    );
#else
    io.Fonts->AddFontDefault();

    GenerateGlyphRanges(ImFontType::Rodin, in_isInstaller);
    GenerateGlyphRanges(ImFontType::NewRodin, in_isInstaller);
    GenerateGlyphRanges(ImFontType::Arial, in_isInstaller);

    io.Fonts->FontBuilderIO = &g_fontBuilderIO;
    io.Fonts->Build();

    FontTexture = std::make_unique<GuestTexture>(ResourceType::Texture);

    uint8_t* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    RenderTextureDesc textureDesc;
    textureDesc.dimension = RenderTextureDimension::TEXTURE_2D;
    textureDesc.width = width;
    textureDesc.height = height;
    textureDesc.depth = 1;
    textureDesc.mipLevels = 1;
    textureDesc.arraySize = 1;
    textureDesc.format = RenderFormat::R8G8B8A8_UNORM;

    const auto pRenderDevice = Video::GetRenderDevice();

    FontTexture->textureHolder = pRenderDevice->createTexture(textureDesc);
    FontTexture->texture = FontTexture->textureHolder.get();

    uint32_t rowPitch = (width * 4 + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
    uint32_t slicePitch = (rowPitch * height + PLACEMENT_ALIGNMENT - 1) & ~(PLACEMENT_ALIGNMENT - 1);
    auto uploadBuffer = pRenderDevice->createBuffer(RenderBufferDesc::UploadBuffer(slicePitch));
    uint8_t* mappedMemory = reinterpret_cast<uint8_t*>(uploadBuffer->map());

    if (rowPitch == (width * 4))
    {
        memcpy(mappedMemory, pixels, slicePitch);
    }
    else
    {
        for (size_t i = 0; i < height; i++)
        {
            memcpy(mappedMemory, pixels, width * 4);
            pixels += width * 4;
            mappedMemory += rowPitch;
        }
    }

    uploadBuffer->unmap();

    Video::ExecuteCopyCommandList([&](RenderCommandList* in_pCommandList)
    {
        in_pCommandList->barriers(RenderBarrierStage::COPY, RenderTextureBarrier(FontTexture->texture, RenderTextureLayout::COPY_DEST));

        in_pCommandList->copyTextureRegion(
            RenderTextureCopyLocation::Subresource(FontTexture->texture, 0),
            RenderTextureCopyLocation::PlacedFootprint(uploadBuffer.get(), RenderFormat::R8G8B8A8_UNORM, width, height, 1, rowPitch / 4, 0));
    });

    FontTexture->layout = RenderTextureLayout::COPY_DEST;

    RenderTextureViewDesc textureViewDesc;
    textureViewDesc.format = textureDesc.format;
    textureViewDesc.dimension = RenderTextureViewDimension::TEXTURE_2D;
    textureViewDesc.mipLevels = 1;
    FontTexture->textureView = FontTexture->texture->createTextureView(textureViewDesc);

    FontTexture->descriptorIndex = g_textureDescriptorAllocator.allocate();
    g_textureDescriptorSet->setTexture(FontTexture->descriptorIndex, FontTexture->texture, RenderTextureLayout::SHADER_READ, FontTexture->textureView.get());
#endif

    io.Fonts->SetTexID(FontTexture.get());

#ifndef ENABLE_IM_FONT_ATLAS_SNAPSHOT
    ImFontAtlasSnapshot snapshot{};
    snapshot.Snap();

    if (FILE* file = fopen("im_font_atlas.bin", "wb"))
    {
        fwrite(snapshot.data.data(), 1, snapshot.data.size(), file);
        fclose(file);
    }

    ddspp::Header header;
    ddspp::HeaderDXT10 headerDX10;
    ddspp::encode_header(ddspp::R8G8B8A8_UNORM, width, height, 1, ddspp::Texture2D, 1, 1, header, headerDX10);

    if (FILE* file = fopen("im_font_atlas.dds", "wb"))
    {
        fwrite(&ddspp::DDS_MAGIC, 4, 1, file);
        fwrite(&header, sizeof(header), 1, file);
        fwrite(&headerDX10, sizeof(headerDX10), 1, file);
        fwrite(pixels, 4, width * height, file);
        fclose(file);
    }
#endif

    IsInitialised = true;
}

ImFont* ImFontAtlasSnapshot::GetFont(ImFontType in_fontType)
{
    const auto& rFileName = g_fonts[in_fontType].pFileName;

    const auto pFontAtlas = ImGui::GetIO().Fonts;

    for (const auto& rConfigData : pFontAtlas->ConfigData)
    {
        if (strstr(rConfigData.Name, rFileName) != nullptr)
        {
            assert(rConfigData.DstFont != nullptr);

            return rConfigData.DstFont;
        }
    }

    LOGFN_ERROR("Failed to locate font in the font atlas: {}", rFileName);

#ifdef ENABLE_IM_FONT_ATLAS_SNAPSHOT
    assert(false && "Failed to locate font in the font atlas.");
#endif

    return nullptr;
}
