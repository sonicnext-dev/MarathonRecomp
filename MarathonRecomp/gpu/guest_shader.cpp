#include "guest_shader.h"
#include "video_internal.h"

#include <kernel/function.h>
#include <kernel/heap.h>
#include <shader/shader_cache.h>
#include <user/config.h>

#include "gpu/shader/shaders.h"

static std::unique_ptr<uint8_t[]> g_shaderCache;

static std::unique_ptr<RenderShader> g_enhancedBurnoutBlurVSShader;
static std::unique_ptr<RenderShader> g_enhancedBurnoutBlurPSShader;

static std::unique_ptr<RenderShader> g_MoviePSShader;

static std::unique_ptr<RenderShader> g_downsamplePSShader;

std::unique_ptr<GuestShader> g_conditionalSurveyPSShader;

void LoadEmbeddedResources()
{
    switch (g_backend)
    {
    case Backend::VULKAN:
        g_shaderCache = std::make_unique<uint8_t[]>(g_spirvCacheDecompressedSize);
        ZSTD_decompress(g_shaderCache.get(), g_spirvCacheDecompressedSize, g_compressedSpirvCache, g_spirvCacheCompressedSize);
        break;
#if defined(MARATHON_RECOMP_D3D12)
    case Backend::D3D12:
        g_shaderCache = std::make_unique<uint8_t[]>(g_dxilCacheDecompressedSize);
        ZSTD_decompress(g_shaderCache.get(), g_dxilCacheDecompressedSize, g_compressedDxilCache, g_dxilCacheCompressedSize);
        break;
#elif defined(MARATHON_RECOMP_METAL)
    case Backend::METAL:
        g_shaderCache = std::make_unique<uint8_t[]>(g_airCacheDecompressedSize);
        ZSTD_decompress(g_shaderCache.get(), g_airCacheDecompressedSize, g_compressedAirCache, g_airCacheCompressedSize);
        break;
#endif
    default:
        assert(false);
    }
}

void CreateReplacementShaders()
{
    g_enhancedBurnoutBlurVSShader = CREATE_SHADER(enhanced_burnout_blur_vs);
    g_enhancedBurnoutBlurPSShader = CREATE_SHADER(enhanced_burnout_blur_ps);
    g_MoviePSShader = CREATE_SHADER(movie_ps);

    g_downsamplePSShader = CREATE_SHADER(downsample_ps);

    g_conditionalSurveyPSShader = std::make_unique<GuestShader>(ResourceType::PixelShader);
    g_conditionalSurveyPSShader->shader = CREATE_SHADER(conditional_survey_ps);
}

RenderShader* GetReplacementShader(const GuestShader* guestShader)
{
    if (guestShader == nullptr)
        return nullptr;

    if (guestShader->shaderCacheEntry != nullptr)
    {
        switch (guestShader->shaderCacheEntry->hash)
        {
        case 0x3687D038CE7D0BEA:
        case 0xB4DA7A442DBB16CC:
            if (Config::RadialBlur == ERadialBlur::Enhanced)
                return g_enhancedBurnoutBlurVSShader.get();
            break;
        case 0xDA58F0110A8595D9:
        case 0x845A4EF989446C01:
            if (Config::RadialBlur == ERadialBlur::Enhanced)
                return g_enhancedBurnoutBlurPSShader.get();
            break;
        case 0x88EB0AA82E4257B0:
            return g_MoviePSShader.get();
        case 0x62573AB4C9EC44EE:
            return g_downsamplePSShader.get();
        default:
            break;
        }
    }

    return nullptr;
}

RenderShader* GetOrLinkShader(GuestShader* guestShader, uint32_t specConstants)
{
    auto* replacementShader = GetReplacementShader(guestShader);
    if (replacementShader != nullptr)
        return replacementShader;

    if (g_backend != Backend::D3D12 ||
        guestShader->shaderCacheEntry == nullptr ||
        guestShader->shaderCacheEntry->specConstantsMask == 0)
    {
        std::lock_guard lock(guestShader->mutex);

        if (guestShader->shader == nullptr)
        {
            assert(guestShader->shaderCacheEntry != nullptr);

            switch (g_backend) {
            case Backend::VULKAN:
            {
                auto compressedSpirvData = g_shaderCache.get() + guestShader->shaderCacheEntry->spirvOffset;

                std::vector<uint8_t> decoded(smolv::GetDecodedBufferSize(compressedSpirvData, guestShader->shaderCacheEntry->spirvSize));
                bool result = smolv::Decode(compressedSpirvData, guestShader->shaderCacheEntry->spirvSize, decoded.data(), decoded.size());
                assert(result);

                guestShader->shader = Video::GetRenderDevice()->createShader(decoded.data(), decoded.size(), "shaderMain", RenderShaderFormat::SPIRV);
                break;
            }
            case Backend::D3D12:
            {
                guestShader->shader = Video::GetRenderDevice()->createShader(g_shaderCache.get() + guestShader->shaderCacheEntry->dxilOffset,
                    guestShader->shaderCacheEntry->dxilSize, "shaderMain", RenderShaderFormat::DXIL);
                break;
            }
            case Backend::METAL:
            {
                guestShader->shader = Video::GetRenderDevice()->createShader(g_shaderCache.get() + guestShader->shaderCacheEntry->airOffset,
                    guestShader->shaderCacheEntry->airSize, "shaderMain", RenderShaderFormat::METAL);
                break;
            }
            }

#ifdef _DEBUG
            guestShader->shader->setName(fmt::format("{}:{:x}", guestShader->shaderCacheEntry->filename, guestShader->shaderCacheEntry->hash));
#endif
        }

        return guestShader->shader.get();
    }

    specConstants &= guestShader->shaderCacheEntry->specConstantsMask;

    RenderShader* shader;
    {
        std::lock_guard lock(guestShader->mutex);
        shader = guestShader->linkedShaders[specConstants].get();
    }

#ifdef MARATHON_RECOMP_D3D12
    if (shader == nullptr)
    {
        static Mutex g_compiledSpecConstantLibraryBlobMutex;
        static ankerl::unordered_dense::map<uint32_t, ComPtr<IDxcBlob>> g_compiledSpecConstantLibraryBlobs;

        thread_local ComPtr<IDxcCompiler3> s_dxcCompiler;
        thread_local ComPtr<IDxcLinker> s_dxcLinker;
        thread_local ComPtr<IDxcUtils> s_dxcUtils;

        wchar_t specConstantsLibName[0x100];
        swprintf_s(specConstantsLibName, L"SpecConstants_%d", specConstants);

        ComPtr<IDxcBlob> specConstantLibraryBlob;
        {
            std::lock_guard lock(g_compiledSpecConstantLibraryBlobMutex);
            specConstantLibraryBlob = g_compiledSpecConstantLibraryBlobs[specConstants];
        }

        if (specConstantLibraryBlob == nullptr)
        {
            if (s_dxcCompiler == nullptr)
            {
                HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(s_dxcCompiler.GetAddressOf()));
                assert(SUCCEEDED(hr) && s_dxcCompiler != nullptr);
            }

            char libraryHlsl[0x100];
            sprintf_s(libraryHlsl, "export uint g_SpecConstants() { return %d; }", specConstants);

            DxcBuffer buffer{};
            buffer.Ptr = libraryHlsl;
            buffer.Size = strlen(libraryHlsl);

            const wchar_t* args[1];
            args[0] = L"-T lib_6_3";

            ComPtr<IDxcResult> result;
            HRESULT hr = s_dxcCompiler->Compile(&buffer, args, std::size(args), nullptr, IID_PPV_ARGS(result.GetAddressOf()));
            assert(SUCCEEDED(hr) && result != nullptr);

            hr = result->GetResult(specConstantLibraryBlob.GetAddressOf());
            assert(SUCCEEDED(hr) && specConstantLibraryBlob != nullptr);

            std::lock_guard lock(g_compiledSpecConstantLibraryBlobMutex);
            g_compiledSpecConstantLibraryBlobs.emplace(specConstants, specConstantLibraryBlob);
        }

        if (s_dxcLinker == nullptr)
        {
            HRESULT hr = DxcCreateInstance(CLSID_DxcLinker, IID_PPV_ARGS(s_dxcLinker.GetAddressOf()));
            assert(SUCCEEDED(hr) && s_dxcLinker != nullptr);
        }

        s_dxcLinker->RegisterLibrary(specConstantsLibName, specConstantLibraryBlob.Get());

        wchar_t shaderLibName[0x100];
        swprintf_s(shaderLibName, L"Shader_%d", guestShader->shaderCacheEntry->dxilOffset);

        ComPtr<IDxcBlobEncoding> shaderLibraryBlob;
        {
            std::lock_guard lock(guestShader->mutex);
            shaderLibraryBlob = guestShader->libraryBlob;
        }

        if (shaderLibraryBlob == nullptr)
        {
            if (s_dxcUtils == nullptr)
            {
                HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(s_dxcUtils.GetAddressOf()));
                assert(SUCCEEDED(hr) && s_dxcUtils != nullptr);
            }

            HRESULT hr = s_dxcUtils->CreateBlobFromPinned(
                g_shaderCache.get() + guestShader->shaderCacheEntry->dxilOffset,
                guestShader->shaderCacheEntry->dxilSize,
                DXC_CP_ACP,
                shaderLibraryBlob.GetAddressOf());

            assert(SUCCEEDED(hr) && shaderLibraryBlob != nullptr);

            std::lock_guard lock(guestShader->mutex);
            guestShader->libraryBlob = shaderLibraryBlob;
        }

        s_dxcLinker->RegisterLibrary(shaderLibName, shaderLibraryBlob.Get());

        const wchar_t* libraryNames[] = { specConstantsLibName, shaderLibName };

        ComPtr<IDxcOperationResult> result;
        HRESULT hr = s_dxcLinker->Link(L"shaderMain", guestShader->type == ResourceType::VertexShader ? L"vs_6_0" : L"ps_6_0",
            libraryNames, std::size(libraryNames), nullptr, 0, result.GetAddressOf());

        assert(SUCCEEDED(hr) && result != nullptr);

        ComPtr<IDxcBlob> blob;
        hr = result->GetResult(blob.GetAddressOf());
        assert(SUCCEEDED(hr) && blob != nullptr);

        {
            std::lock_guard lock(guestShader->mutex);

            auto& linkedShader = guestShader->linkedShaders[specConstants];
            if (linkedShader == nullptr)
            {
                linkedShader = Video::GetRenderDevice()->createShader(blob->GetBufferPointer(), blob->GetBufferSize(), "shaderMain", RenderShaderFormat::DXIL);
                guestShader->shaderBlobs.push_back(std::move(blob));
            }

            shader = linkedShader.get();

#ifdef _DEBUG
            shader->setName(fmt::format("{}:{:x}", guestShader->shaderCacheEntry->filename, guestShader->shaderCacheEntry->hash));
#endif
        }
    }
#endif

    return shader;
}

static ShaderCacheEntry* FindShaderCacheEntry(XXH64_hash_t hash)
{
    auto end = g_shaderCacheEntries + g_shaderCacheEntryCount;
    auto findResult = std::lower_bound(g_shaderCacheEntries, end, hash, [](const ShaderCacheEntry& lhs, XXH64_hash_t rhs)
        {
            return lhs.hash < rhs;
        });

    return findResult != end && findResult->hash == hash ? findResult : nullptr;
}

GuestShader* GetOrCreateShader(XXH64_hash_t hash, ResourceType resourceType) {
    auto findResult = FindShaderCacheEntry(hash);
    GuestShader* shader = nullptr;

    if (findResult == nullptr)
    {
        LOGF_WARNING("Shader not found by hash: {:x}", hash);
        LOG_WARNING("Perhaps the path to the required shader will be printed before this error");
        __builtin_trap();
    }
    if (findResult != nullptr)
    {
        if (findResult->guestShader == nullptr)
        {
            shader = g_userHeap.AllocPhysical<GuestShader>(resourceType);
            shader->shaderCacheEntry = findResult;
#ifdef ASYNC_PSO_DEBUG
            shader->name = findResult->filename;
#endif

            findResult->guestShader = shader;
        }
        else
        {
            shader = findResult->guestShader;
        }
    }

    if (shader == nullptr)
        shader = g_userHeap.AllocPhysical<GuestShader>(resourceType);
    else
        shader->AddRef();

    return shader;
}

static GuestShader* CreateShader(const be<uint32_t>* function, ResourceType resourceType)
{
    XXH64_hash_t hash = XXH3_64bits(function, function[1] + function[2]);
    return GetOrCreateShader(hash, resourceType);
}

static GuestShader* CreateVertexShader(const be<uint32_t>* function)
{
    return CreateShader(function, ResourceType::VertexShader);
}

static GuestShader* CreatePixelShader(const be<uint32_t>* function)
{
    return CreateShader(function, ResourceType::PixelShader);
}

GUEST_FUNCTION_HOOK(sub_82548700, CreateVertexShader);
GUEST_FUNCTION_HOOK(sub_82548608, CreatePixelShader);
