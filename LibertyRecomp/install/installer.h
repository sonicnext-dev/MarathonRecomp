#pragma once

#include <span>
#include <set>

#include "virtual_file_system.h"

enum class DLC {
    Unknown,
    EpisodeSonic,
    EpisodeShadow,
    EpisodeSilver,
    EpisodeAmigo,
    MissionSonic,
    MissionShadow,
    MissionSilver,
    Count = MissionSilver
};

struct Journal
{
    enum class Result
    {
        Success,
        Cancelled,
        VirtualFileSystemFailed,
        DirectoryCreationFailed,
        FileMissing,
        FileReadFailed,
        FileHashFailed,
        FileCreationFailed,
        FileWriteFailed,
        ValidationFileMissing,
        DLCParsingFailed,
        UnknownDLCType
    };

    uint64_t progressCounter = 0;
    uint64_t progressTotal = 0;
    std::list<std::filesystem::path> createdFiles;
    std::set<std::filesystem::path> createdDirectories;
    Result lastResult = Result::Success;
    std::string lastErrorMessage;
};

using FilePair = std::pair<const char *, uint32_t>;

struct Installer
{
    struct Input
    {
        std::filesystem::path gameSource;
        std::list<std::filesystem::path> dlcSources;
    };

    struct DLCSource {
        std::unique_ptr<VirtualFileSystem> sourceVfs;
        std::span<const FilePair> filePairs;
        const uint64_t *fileHashes = nullptr;
        std::string targetSubDirectory;
    };

    struct Sources 
    {
        std::unique_ptr<VirtualFileSystem> game;
        std::vector<DLCSource> dlc;
        uint64_t totalSize = 0;
    };

    static bool checkGameInstall(const std::filesystem::path &baseDirectory, std::filesystem::path &modulePath);
    static bool checkDLCInstall(const std::filesystem::path &baseDirectory, DLC dlc);
    static bool checkAllDLC(const std::filesystem::path &baseDirectory);
    static bool checkInstallIntegrity(const std::filesystem::path &baseDirectory, Journal &journal, const std::function<bool()> &progressCallback);
    static bool computeTotalSize(std::span<const FilePair> filePairs, const uint64_t *fileHashes, VirtualFileSystem &sourceVfs, Journal &journal, uint64_t &totalSize);
    static bool checkFiles(std::span<const FilePair> filePairs, const uint64_t *fileHashes, const std::filesystem::path &targetDirectory, Journal &journal, const std::function<bool()> &progressCallback, bool checkSizeOnly);
    static bool copyFiles(std::span<const FilePair> filePairs, const uint64_t *fileHashes, VirtualFileSystem &sourceVfs, const std::filesystem::path &targetDirectory, const std::string &validationFile, bool skipHashChecks, Journal &journal, const std::function<bool()> &progressCallback);
    static bool parseContent(const std::filesystem::path &sourcePath, std::unique_ptr<VirtualFileSystem> &targetVfs, Journal &journal);
    static bool parseSources(const Input &input, Journal &journal, Sources &sources);
    static bool install(const Sources &sources, const std::filesystem::path &targetDirectory, bool skipHashChecks, Journal &journal, std::chrono::seconds endWaitTime, const std::function<bool()> &progressCallback);
    static void rollback(Journal &journal);

    // Convenience method for checking if the specified file contains the game. This should be used when the user selects the file.
    static bool parseGame(const std::filesystem::path &sourcePath);

    // Convenience method for the installer to check which DLC the file that was specified corresponds to. This should be used when the user selects the file.
    static DLC parseDLC(const std::filesystem::path &sourcePath);
};
