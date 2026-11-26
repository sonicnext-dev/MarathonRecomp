#pragma once

#include <nlohmann/json.hpp>
#include <patches/csd_modifier.h>
#include <filesystem>

class CsdModifierManager
{
public:
    static inline constexpr size_t Version = 1;

    enum class Status
    {
        Success,
        IOError,
        ParseError,
        MissingFile,
        MissingVersion,
        UnsupportedVersion,
        MissingModifiers
    };

private:
    static inline const std::array<const char*, 7> ms_StatusMessages =
    {
        "The operation completed successfully.",
        "An I/O error occurred upon attempting to read the specified JSON file.",
        "An error occurred upon parsing the specified JSON file.",
        "The specified JSON file could not be found.",
        "The specified JSON file does not contain a version field.",
        "The specified JSON file uses an unsupported version that cannot be read.",
        "The specified JSON file does not contain a modifiers array."
    };

public:
    static Status Read(const nlohmann::json& in_rReader);
    static Status Read(const std::filesystem::path& in_rPath);
    static Status Read();
    static Status Write(const std::filesystem::path& in_rPath);

    static const char* GetStatusMessage(const Status in_status);

    static void Add(const CsdModifier& in_rModifier);
    static bool Remove(const std::string_view in_key);

    static CsdModifier* Find(uint32_t in_data);

    static void DrawDebug();
};
