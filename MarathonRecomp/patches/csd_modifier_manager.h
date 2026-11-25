#pragma once

#include <nlohmann/json.hpp>
#include <patches/csd_modifier.h>
#include <filesystem>

class CsdModifierManager
{
public:
    static bool Read(const nlohmann::json& in_rReader);
    static bool Write(const std::filesystem::path& in_rPath);

    static CsdModifier* Find(uint32_t in_data);

    static void DrawDebug();
};
