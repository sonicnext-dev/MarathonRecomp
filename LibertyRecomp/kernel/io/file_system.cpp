#include "file_system.h"
#include <cpu/guest_thread.h>
#include <cstdio>
#include <kernel/xam.h>
#include <kernel/xdm.h>
#include <kernel/function.h>
#include <mod/mod_loader.h>
#include <os/logger.h>
#include <user/config.h>
#include <user/paths.h>
#include <stdafx.h>

struct FileHandle : KernelObject
{
    std::fstream stream;
    std::filesystem::path path;
};

struct FindHandle : KernelObject
{
    std::error_code ec;
    ankerl::unordered_dense::map<std::u8string, std::pair<size_t, bool>> searchResult; // Relative path, file size, is directory
    decltype(searchResult)::iterator iterator;

    FindHandle(const std::string_view& path)
    {
        auto addDirectory = [&](const std::filesystem::path& directory)
            {
                for (auto& entry : std::filesystem::directory_iterator(directory, ec))
                {
                    std::u8string relativePath = entry.path().lexically_relative(directory).u8string();
                    searchResult.emplace(relativePath, std::make_pair(entry.is_directory(ec) ? 0 : entry.file_size(ec), entry.is_directory(ec)));
                }
            };

        std::string_view pathNoPrefix = path;
        size_t index = pathNoPrefix.find(":\\");
        if (index != std::string_view::npos)
            pathNoPrefix.remove_prefix(index + 2);

        // Force add a work folder to let the game see the files in mods,
        // if by some rare chance the user has no DLC or update files.
        if (pathNoPrefix.empty())
            searchResult.emplace(u8"work", std::make_pair(0, true));

        // Look for only work folder in mod folders, AR files cause issues.
        if (pathNoPrefix.starts_with("work"))
        {
            std::string pathStr(pathNoPrefix);
            std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

            for (size_t i = 0; ; i++)
            {
                auto* includeDirs = ModLoader::GetIncludeDirectories(i);
                if (includeDirs == nullptr)
                    break;

                for (auto& includeDir : *includeDirs)
                    addDirectory(includeDir / pathStr);
            }
        }

        addDirectory(FileSystem::ResolvePath(path, false));

        iterator = searchResult.begin();
    }

    void fillFindData(WIN32_FIND_DATAA* lpFindFileData)
    {
        if (iterator->second.second)
            lpFindFileData->dwFileAttributes = ByteSwap(FILE_ATTRIBUTE_DIRECTORY);
        else
            lpFindFileData->dwFileAttributes = ByteSwap(FILE_ATTRIBUTE_NORMAL);

        strncpy(lpFindFileData->cFileName, (const char *)(iterator->first.c_str()), sizeof(lpFindFileData->cFileName));
        lpFindFileData->nFileSizeLow = ByteSwap(uint32_t(iterator->second.first >> 32U));
        lpFindFileData->nFileSizeHigh = ByteSwap(uint32_t(iterator->second.first));
        lpFindFileData->ftCreationTime = {};
        lpFindFileData->ftLastAccessTime = {};
        lpFindFileData->ftLastWriteTime = {};
    }
};

FileHandle* XCreateFileA
(
    const char* lpFileName,
    uint32_t dwDesiredAccess,
    uint32_t dwShareMode,
    void* lpSecurityAttributes,
    uint32_t dwCreationDisposition,
    uint32_t dwFlagsAndAttributes
)
{
    assert(((dwDesiredAccess & ~(GENERIC_READ | GENERIC_WRITE | FILE_READ_DATA)) == 0) && "Unknown desired access bits.");
    assert(((dwShareMode & ~(FILE_SHARE_READ | FILE_SHARE_WRITE)) == 0) && "Unknown share mode bits.");
    assert(((dwCreationDisposition & ~(CREATE_NEW | CREATE_ALWAYS)) == 0) && "Unknown creation disposition bits.");

    std::filesystem::path filePath = FileSystem::ResolvePath(lpFileName, true);
    std::fstream fileStream;
    std::ios::openmode fileOpenMode = std::ios::binary;
    if (dwDesiredAccess & (GENERIC_READ | FILE_READ_DATA))
    {
        fileOpenMode |= std::ios::in;
    }

    if (dwDesiredAccess & GENERIC_WRITE)
    {
        fileOpenMode |= std::ios::out;
    }

    fileStream.open(filePath, fileOpenMode);

    if (!fileStream.is_open()) {
        std::filesystem::path cachedPath = FindInPathCache(filePath.string());
        if (!cachedPath.empty()) {
            fileStream.open(cachedPath, fileOpenMode);
        }
    }

    if (!fileStream.is_open())
    {
#ifdef _WIN32
        GuestThread::SetLastError(GetLastError());
#else
        switch (errno)
        {
        case EACCES:
            GuestThread::SetLastError(ERROR_ACCESS_DENIED);
            break;
        case EEXIST:
            GuestThread::SetLastError(ERROR_FILE_EXISTS);
            break;
        case ENOENT:
        default: // Use ERROR_PATH_NOT_FOUND as a catch-all for other errors.
            GuestThread::SetLastError(ERROR_PATH_NOT_FOUND);
            break;
        }
#endif
        return GetInvalidKernelObject<FileHandle>();
    }

    FileHandle *fileHandle = CreateKernelObject<FileHandle>();
    fileHandle->stream = std::move(fileStream);
    fileHandle->path = std::move(filePath);
    return fileHandle;
}

static uint32_t XGetFileSizeA(FileHandle* hFile, be<uint32_t>* lpFileSizeHigh)
{
    std::error_code ec;
    auto fileSize = std::filesystem::file_size(hFile->path, ec);
    if (!ec)
    {
        if (lpFileSizeHigh != nullptr)
        {
            *lpFileSizeHigh = uint32_t(fileSize >> 32U);
        }
    
        return (uint32_t)(fileSize);
    }

    return INVALID_FILE_SIZE;
}

uint32_t XGetFileSizeExA(FileHandle* hFile, LARGE_INTEGER* lpFileSize)
{
    std::error_code ec;
    auto fileSize = std::filesystem::file_size(hFile->path, ec);
    if (!ec)
    {
        if (lpFileSize != nullptr)
        {
            lpFileSize->QuadPart = ByteSwap(fileSize);
        }

        return TRUE;
    }

    return FALSE;
}

uint32_t XReadFile
(
    FileHandle* hFile,
    void* lpBuffer,
    uint32_t nNumberOfBytesToRead,
    be<uint32_t>* lpNumberOfBytesRead,
    XOVERLAPPED* lpOverlapped
)
{
    uint32_t result = FALSE;
    if (lpOverlapped != nullptr)
    {
        std::streamoff streamOffset = lpOverlapped->Offset + (std::streamoff(lpOverlapped->OffsetHigh.get()) << 32U);
        hFile->stream.clear();
        hFile->stream.seekg(streamOffset, std::ios::beg);
        if (hFile->stream.bad())
        {
            return FALSE;
        }
    }

    uint32_t numberOfBytesRead;
    hFile->stream.read((char *)(lpBuffer), nNumberOfBytesToRead);
    if (!hFile->stream.bad())
    {
        numberOfBytesRead = uint32_t(hFile->stream.gcount());
        result = TRUE;
    }

    if (result)
    {
        if (lpOverlapped != nullptr)
        {
            lpOverlapped->Internal = 0;
            lpOverlapped->InternalHigh = numberOfBytesRead;
        }
        else if (lpNumberOfBytesRead != nullptr)
        {
            *lpNumberOfBytesRead = numberOfBytesRead;
        }
    }

    return result;
}

uint32_t XSetFilePointer(FileHandle* hFile, int32_t lDistanceToMove, be<int32_t>* lpDistanceToMoveHigh, uint32_t dwMoveMethod)
{
    int32_t distanceToMoveHigh = lpDistanceToMoveHigh ? lpDistanceToMoveHigh->get() : 0;
    std::streamoff streamOffset = lDistanceToMove + (std::streamoff(distanceToMoveHigh) << 32U);
    std::fstream::seekdir streamSeekDir = {};
    switch (dwMoveMethod)
    {
    case FILE_BEGIN:
        streamSeekDir = std::ios::beg;
        break;
    case FILE_CURRENT:
        streamSeekDir = std::ios::cur;
        break;
    case FILE_END:
        streamSeekDir = std::ios::end;
        break;
    default:
        assert(false && "Unknown move method.");
        break;
    }

    hFile->stream.clear();
    hFile->stream.seekg(streamOffset, streamSeekDir);
    if (hFile->stream.bad())
    {
        return INVALID_SET_FILE_POINTER;
    }

    std::streampos streamPos = hFile->stream.tellg();
    if (lpDistanceToMoveHigh != nullptr)
        *lpDistanceToMoveHigh = int32_t(streamPos >> 32U);

    return uint32_t(streamPos);
}

uint32_t XSetFilePointerEx(FileHandle* hFile, int32_t lDistanceToMove, LARGE_INTEGER* lpNewFilePointer, uint32_t dwMoveMethod)
{
    std::fstream::seekdir streamSeekDir = {};
    switch (dwMoveMethod)
    {
    case FILE_BEGIN:
        streamSeekDir = std::ios::beg;
        break;
    case FILE_CURRENT:
        streamSeekDir = std::ios::cur;
        break;
    case FILE_END:
        streamSeekDir = std::ios::end;
        break;
    default:
        assert(false && "Unknown move method.");
        break;
    }

    hFile->stream.clear();
    hFile->stream.seekg(lDistanceToMove, streamSeekDir);
    if (hFile->stream.bad())
    {
        return FALSE;
    }

    if (lpNewFilePointer != nullptr)
    {
        lpNewFilePointer->QuadPart = ByteSwap(int64_t(hFile->stream.tellg()));
    }

    return TRUE;
}

FindHandle* XFindFirstFileA(const char* lpFileName, WIN32_FIND_DATAA* lpFindFileData)
{
    LOGF_IMPL(Utility, "XFindFirstFileA", "path: '{}'", lpFileName);
    std::string_view path = lpFileName;
    if (path.find("\\*") == (path.size() - 2) || path.find("/*") == (path.size() - 2))
    {
        path.remove_suffix(1);
    }
    else if (path.find("\\*.*") == (path.size() - 4) || path.find("/*.*") == (path.size() - 4))
    {
        path.remove_suffix(3);
    }
    else
    {
        assert(!std::filesystem::path(path).has_extension() && "Unknown search pattern.");
    }

    FindHandle findHandle(path);

    if (findHandle.searchResult.empty())
        return GetInvalidKernelObject<FindHandle>();

    findHandle.fillFindData(lpFindFileData);

    return CreateKernelObject<FindHandle>(std::move(findHandle));
}

uint32_t XFindNextFileA(FindHandle* Handle, WIN32_FIND_DATAA* lpFindFileData)
{
    Handle->iterator++;

    if (Handle->iterator == Handle->searchResult.end())
    {
        return FALSE;
    }
    else
    {
        Handle->fillFindData(lpFindFileData);
        return TRUE;
    }
}

uint32_t XReadFileEx(FileHandle* hFile, void* lpBuffer, uint32_t nNumberOfBytesToRead, XOVERLAPPED* lpOverlapped, uint32_t lpCompletionRoutine)
{
    uint32_t result = FALSE;
    uint32_t numberOfBytesRead;
    std::streamoff streamOffset = lpOverlapped->Offset + (std::streamoff(lpOverlapped->OffsetHigh.get()) << 32U);
    hFile->stream.clear();
    hFile->stream.seekg(streamOffset, std::ios::beg);
    if (hFile->stream.bad())
        return FALSE;

    hFile->stream.read((char *)(lpBuffer), nNumberOfBytesToRead);
    if (!hFile->stream.bad())
    {
        numberOfBytesRead = uint32_t(hFile->stream.gcount());
        result = TRUE;
    }

    if (result)
    {
        lpOverlapped->Internal = 0;
        lpOverlapped->InternalHigh = numberOfBytesRead;
    }

    return result;
}

uint32_t XGetFileAttributesA(const char* lpFileName)
{
    std::filesystem::path filePath = FileSystem::ResolvePath(lpFileName, true);
    if (std::filesystem::is_directory(filePath))
        return FILE_ATTRIBUTE_DIRECTORY;
    else if (std::filesystem::is_regular_file(filePath))
        return FILE_ATTRIBUTE_NORMAL;
    else
        return INVALID_FILE_ATTRIBUTES;
}

uint32_t XWriteFile(FileHandle* hFile, const void* lpBuffer, uint32_t nNumberOfBytesToWrite, be<uint32_t>* lpNumberOfBytesWritten, void* lpOverlapped)
{
    assert(lpOverlapped == nullptr && "Overlapped not implemented.");

    hFile->stream.write((const char *)(lpBuffer), nNumberOfBytesToWrite);
    if (hFile->stream.bad())
        return FALSE;

    if (lpNumberOfBytesWritten != nullptr)
        *lpNumberOfBytesWritten = uint32_t(hFile->stream.gcount());

    return TRUE;
}

std::filesystem::path FileSystem::ResolvePath(const std::string_view& path, bool checkForMods)
{
    // Skip empty paths - these are likely null pointers or uninitialized strings
    if (path.empty())
    {
        return std::filesystem::path();
    }
    
    // Reject paths that are just slashes (would resolve to root filesystem which is wrong)
    bool isOnlySlashes = true;
    for (char c : path) {
        if (c != '\\' && c != '/') {
            isOnlySlashes = false;
            break;
        }
    }
    if (isOnlySlashes) {
        LOG_UTILITY("Path is only slashes, returning empty path to prevent root filesystem access");
        return std::filesystem::path{};
    }
    
    LOGF_IMPL(Utility, "Game", "Loading file: \"{}\"", path);
    
    // Static path to extracted folder (computed once)
    static std::filesystem::path s_extractedPath = GetGamePath() / "extracted";
    
    // Special handling for shader paths - redirect to extracted shader location
    std::string_view pathView = path;
    
    // Validate path doesn't contain garbage characters
    bool hasGarbage = false;
    for (char c : pathView) {
        if ((unsigned char)c > 127 || (c < 32 && c != 0)) {
            hasGarbage = true;
            break;
        }
    }
    
    // Storage for cleaned-up path if we need to modify it
    thread_local std::string s_cleanedPath;
    
    if (hasGarbage) {
        // If the path starts with "fxl_final\" but has garbage, try to use a default shader
        // This handles the case where shader name is read from uninitialized memory
        size_t fxlPos = pathView.find("fxl_final");
        if (fxlPos != std::string_view::npos) {
            // Extract just the fxl_final part and use gta_default.fxc as fallback
            s_cleanedPath = "fxl_final\\gta_default.fxc";
            LOGF_IMPL(Utility, "Game", "Path had garbage, using fallback shader: {}", s_cleanedPath);
            pathView = s_cleanedPath;
            // Continue with normal path resolution below
        } else {
            LOG_UTILITY("Path contains invalid characters, returning empty path");
            return std::filesystem::path{};
        }
    }
    
    // Handle shader paths - but NOT if path starts with a drive prefix like "game:\"
    // because those need to be resolved through the root path system first
    bool hasRootPrefix = (pathView.find(":\\") != std::string_view::npos);
    
    if (!hasRootPrefix && 
        (pathView.find("fxl_final") != std::string_view::npos ||
         pathView.find("shaders") != std::string_view::npos))
    {
        // Strip leading slash or backslash
        while (!pathView.empty() && (pathView.front() == '\\' || pathView.front() == '/'))
            pathView = pathView.substr(1);
        
        // Strip trailing slash or backslash
        while (!pathView.empty() && (pathView.back() == '\\' || pathView.back() == '/'))
            pathView = pathView.substr(0, pathView.size() - 1);
        
        // Convert Windows path separators to Unix for the path component
        std::string cleanPath(pathView);
        std::replace(cleanPath.begin(), cleanPath.end(), '\\', '/');
            
        // Build path to extracted shaders (using new extracted folder structure)
        std::filesystem::path shaderPath = s_extractedPath / "common" / "shaders" / cleanPath;
        
        if (std::filesystem::exists(shaderPath))
        {
            LOGF_IMPL(Utility, "Game", "Shader path resolved to: \"{}\"", shaderPath.string());
            return shaderPath;
        }
        
        // Fallback to old RPF DUMP location
        static std::filesystem::path s_rpfDumpPath = GetGamePath() / "RPF DUMP" / "Common RPF Dump" / "shaders";
        shaderPath = s_rpfDumpPath / cleanPath;
        LOGF_IMPL(Utility, "Game", "Trying RPF dump path: \"{}\"", s_rpfDumpPath.string());
        
        LOGF_IMPL(Utility, "Game", "Shader path resolved to: \"{}\"", shaderPath.string());
        return shaderPath;
    }
    
    if (checkForMods)
    {
        std::filesystem::path resolvedPath = ModLoader::ResolvePath(path);

        if (!resolvedPath.empty())
        {
            if (ModLoader::s_isLogTypeConsole)
                LOGF_IMPL(Utility, "Mod Loader", "Loading file: \"{}\"", reinterpret_cast<const char*>(resolvedPath.u8string().c_str()));

            return resolvedPath;
        }
    }

    thread_local std::string builtPath;
    builtPath.clear();

    size_t index = path.find(":\\");
    if (index != std::string::npos)
    {
        // rooted folder, handle direction
        const std::string_view root = path.substr(0, index);
        const auto newRoot = XamGetRootPath(root);

        // Force shader loads from extracted folder even when 'game' root is registered.
        {
            std::string rootStr(root);
            std::transform(rootStr.begin(), rootStr.end(), rootStr.begin(), ::tolower);
            if (rootStr == "game")
            {
                std::string remainder(path.substr(index + 2));
                std::string remainderLower = remainder;
                std::transform(remainderLower.begin(), remainderLower.end(), remainderLower.begin(), ::tolower);

                while (!remainderLower.empty() && (remainderLower[0] == '\\' || remainderLower[0] == '/'))
                    remainderLower.erase(0, 1);

                if (remainderLower.find("fxl_final") == 0)
                {
                    std::string subpath = remainderLower.substr(9); // skip "fxl_final"
                    std::filesystem::path shaderPath = s_extractedPath / "common" / "shaders" / "fxl_final";
                    builtPath = (const char*)shaderPath.u8string().c_str();
                    builtPath += subpath;
                    std::replace(builtPath.begin(), builtPath.end(), '\\', '/');
                    LOGF_IMPL(Utility, "Game", "Redirecting game:\\fxl_final to: {}", builtPath);
                    return std::filesystem::path(std::u8string_view((const char8_t*)builtPath.c_str()));
                }
            }
        }

        if (!newRoot.empty())
        {
            builtPath += newRoot;
            builtPath += '/';
        }
        else
        {
            // Root not found in registry - try using extracted folder as fallback
            // This handles paths like "game:\\common.rpf" -> extracted/common/
            // or "platform:\\data\\..." -> extracted/xbox360/data/...
            std::string rootStr(root);
            std::transform(rootStr.begin(), rootStr.end(), rootStr.begin(), ::tolower);
            
            if (rootStr == "game")
            {
                // Check if this is a path to fxl_final shaders - redirect to extracted
                std::string remainder(path.substr(index + 2));
                std::string remainderLower = remainder;
                std::transform(remainderLower.begin(), remainderLower.end(), remainderLower.begin(), ::tolower);
                
                // Strip leading slashes from remainder for comparison
                while (!remainderLower.empty() && (remainderLower[0] == '\\' || remainderLower[0] == '/'))
                    remainderLower.erase(0, 1);
                
                if (remainderLower.find("fxl_final") == 0)
                {
                    // Redirect game:\fxl_final\... to extracted/common/shaders/fxl_final/...
                    // Strip fxl_final from remainder and build full path
                    std::string subpath = remainderLower.substr(9); // skip "fxl_final"
                    std::filesystem::path shaderPath = s_extractedPath / "common" / "shaders" / "fxl_final";
                    builtPath = (const char*)shaderPath.u8string().c_str();
                    builtPath += subpath;
                    std::replace(builtPath.begin(), builtPath.end(), '\\', '/');
                    LOGF_IMPL(Utility, "Game", "Redirecting game:\\fxl_final to: {}", builtPath);
                    return std::filesystem::path(std::u8string_view((const char8_t*)builtPath.c_str()));
                }
                else if (remainderLower.empty() || remainderLower == "\\" || remainderLower == "/")
                {
                    // game:\ root - map to game folder for RPF mount detection
                    builtPath += (const char*)(GetGamePath() / "game").u8string().c_str();
                    builtPath += '/';
                }
                else if (remainderLower.find("common") == 0)
                {
                    // game:\common\... -> extracted/common/...
                    builtPath += (const char*)s_extractedPath.u8string().c_str();
                    builtPath += '/';
                    LOGF_IMPL(Utility, "Game", "Redirecting game:\\common to extracted: {}", s_extractedPath.string());
                }
                else if (remainderLower.find("xbox360") == 0 || remainderLower.find("platform") == 0)
                {
                    // game:\xbox360\... or game:\platform\... -> extracted/xbox360/...
                    // Strip the xbox360/platform prefix and redirect
                    size_t prefixEnd = remainderLower.find_first_of("\\/", 1);
                    if (prefixEnd == std::string::npos) prefixEnd = remainder.size();
                    std::string subpath = remainder.substr(prefixEnd);
                    builtPath += (const char*)(s_extractedPath / "xbox360").u8string().c_str();
                    // Don't add remainder - we'll handle it specially
                    builtPath += subpath;
                    std::replace(builtPath.begin(), builtPath.end(), '\\', '/');
                    LOGF_IMPL(Utility, "Game", "Redirecting game:\\xbox360 to extracted: {}", builtPath);
                    return std::filesystem::path(std::u8string_view((const char8_t*)builtPath.c_str()));
                }
                else
                {
                    // Other game:\ paths - try game folder first, then extracted
                    builtPath += (const char*)(GetGamePath() / "game").u8string().c_str();
                    builtPath += '/';
                }
            }
            else
            {
                // Try extracted folder with the root name
                std::filesystem::path extractedRoot = s_extractedPath / rootStr;
                if (std::filesystem::exists(extractedRoot))
                {
                    builtPath += (const char*)extractedRoot.u8string().c_str();
                    builtPath += '/';
                    LOGF_IMPL(Utility, "Game", "Using extracted fallback for {}: -> {}", root, extractedRoot.string());
                }
            }
        }
        
        builtPath += path.substr(index + 2);
    }
    else
    {
        builtPath += path;
    }

    std::replace(builtPath.begin(), builtPath.end(), '\\', '/');
    
    // Strip trailing slashes (except for root paths)
    while (builtPath.size() > 1 && builtPath.back() == '/')
        builtPath.pop_back();

    std::filesystem::path resolvedPath = std::u8string_view((const char8_t*)builtPath.c_str());
    if (std::filesystem::exists(resolvedPath))
        return resolvedPath;

    // Path cache fallback (built from game/ and RPF DUMP/ at startup).
    {
        std::filesystem::path cachedPath = FindInPathCache(resolvedPath.string());
        if (!cachedPath.empty() && std::filesystem::exists(cachedPath))
            return cachedPath;
    }

    // RPF DUMP fallback: map common/platform/audio extracted trees.
    // This keeps runtime simple (no RPF parsing) while still satisfying file loads.
    {
        static const std::filesystem::path s_projectRoot = GetGamePath();
        static const std::filesystem::path s_gameRoot = s_projectRoot / "game";
        static const std::filesystem::path s_rpfDumpRoot = s_projectRoot / "RPF DUMP";

        auto tryReturn = [&](const std::filesystem::path& candidate) -> std::filesystem::path
        {
            if (!candidate.empty() && std::filesystem::exists(candidate))
                return candidate;

            // As a last chance, consult the cache for the candidate too.
            std::filesystem::path cached = FindInPathCache(candidate.string());
            if (!cached.empty() && std::filesystem::exists(cached))
                return cached;

            return {};
        };

        // game/common/* -> RPF DUMP/common/*
        const std::filesystem::path gameCommon = s_gameRoot / "common";
        if (resolvedPath.native().starts_with(gameCommon.native()))
        {
            std::error_code ec;
            std::filesystem::path rel = std::filesystem::relative(resolvedPath, gameCommon, ec);
            if (!ec)
            {
                if (auto p = tryReturn(s_rpfDumpRoot / "common" / rel); !p.empty())
                    return p;
                if (auto p = tryReturn(s_rpfDumpRoot / "Common RPF Dump" / rel); !p.empty())
                    return p;
            }
        }

        // game/xbox360/* -> RPF DUMP/xbox360/*
        const std::filesystem::path gameXbox360 = s_gameRoot / "xbox360";
        if (resolvedPath.native().starts_with(gameXbox360.native()))
        {
            std::error_code ec;
            std::filesystem::path rel = std::filesystem::relative(resolvedPath, gameXbox360, ec);
            if (!ec)
            {
                if (auto p = tryReturn(s_rpfDumpRoot / "xbox360" / rel); !p.empty())
                    return p;

                // Nested playerped.rpf extractions often land in flattened folders.
                // xbox360/models/cdimages/playerped/* -> RPF DUMP/models_cdimages_playerped/* (or xbox360_models_cdimages_playerped)
                if (rel.native().starts_with(std::filesystem::path("models/cdimages/playerped").native()))
                {
                    std::filesystem::path rel2 = std::filesystem::relative(rel, std::filesystem::path("models/cdimages/playerped"), ec);
                    if (!ec)
                    {
                        if (auto p = tryReturn(s_rpfDumpRoot / "models_cdimages_playerped" / rel2); !p.empty())
                            return p;
                        if (auto p = tryReturn(s_rpfDumpRoot / "xbox360_models_cdimages_playerped" / rel2); !p.empty())
                            return p;
                        if (auto p = tryReturn(s_rpfDumpRoot / "nested_XBOX 360 RPF DUMP_models_cdimages_playerped" / rel2); !p.empty())
                            return p;
                    }
                }

                // Nested audio SFX packs are commonly dumped as folders like:
                //   RPF DUMP/xbox360_audio_sfx_radio_liberty_rock/
                // while the game expects:
                //   xbox360/audio/sfx/radio_liberty_rock/...
                if (rel.native().starts_with(std::filesystem::path("audio/sfx").native()))
                {
                    // Extract the sfx pack name (first component after audio/sfx)
                    auto relIter = rel.begin();
                    if (relIter != rel.end()) ++relIter; // audio
                    if (relIter != rel.end()) ++relIter; // sfx
                    if (relIter != rel.end())
                    {
                        const std::string packName = relIter->string();
                        ++relIter;

                        std::filesystem::path remaining;
                        for (; relIter != rel.end(); ++relIter)
                            remaining /= *relIter;

                        const std::filesystem::path flatDir = s_rpfDumpRoot / ("xbox360_audio_sfx_" + packName);
                        if (auto p = tryReturn(flatDir / remaining); !p.empty())
                            return p;
                    }
                }
            }
        }

        // game/audio/* -> RPF DUMP/audio/* (or AUDIO RPF DUMP)
        const std::filesystem::path gameAudio = s_gameRoot / "audio";
        if (resolvedPath.native().starts_with(gameAudio.native()))
        {
            std::error_code ec;
            std::filesystem::path rel = std::filesystem::relative(resolvedPath, gameAudio, ec);
            if (!ec)
            {
                if (auto p = tryReturn(s_rpfDumpRoot / "audio" / rel); !p.empty())
                    return p;
                if (auto p = tryReturn(s_rpfDumpRoot / "AUDIO RPF DUMP" / rel); !p.empty())
                    return p;
            }
        }
    }

    return resolvedPath;
}

GUEST_FUNCTION_HOOK(sub_82537400, XCreateFileA); // replaced
GUEST_FUNCTION_HOOK(sub_826FD090, XGetFileSizeA); // replaced
GUEST_FUNCTION_HOOK(sub_826FDC88, XGetFileSizeExA); // replaced
GUEST_FUNCTION_HOOK(sub_82537118, XReadFile); // replaced
GUEST_FUNCTION_HOOK(sub_825372B8, XSetFilePointer); // replaced
// GUEST_FUNCTION_HOOK(sub_831CE888, XSetFilePointerEx);
GUEST_FUNCTION_HOOK(sub_826F2570, XFindFirstFileA); // replaced
GUEST_FUNCTION_HOOK(sub_826FD2B8, XFindNextFileA); // replaced
// GUEST_FUNCTION_HOOK(sub_831CDF40, XReadFileEx);
GUEST_FUNCTION_HOOK(sub_826FD250, XGetFileAttributesA); // replaced
// GUEST_FUNCTION_HOOK(sub_831CE3F8, XCreateFileA);
GUEST_FUNCTION_HOOK(sub_826FCBD0, XWriteFile); // replaced
