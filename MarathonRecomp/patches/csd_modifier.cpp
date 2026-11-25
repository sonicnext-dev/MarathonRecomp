#include "csd_modifier.h"
#include <patches/aspect_ratio_patches.h>
#include <ui/black_bar.h>
#include <user/config.h>

std::map<uint64_t, const char*> g_CsdFlagNames =
{
    { CSD_ALIGN_CENTER, "CSD_ALIGN_CENTER" },
    { CSD_ALIGN_TOP, "CSD_ALIGN_TOP" },
    { CSD_ALIGN_LEFT, "CSD_ALIGN_LEFT" },
    { CSD_ALIGN_BOTTOM, "CSD_ALIGN_BOTTOM" },
    { CSD_ALIGN_RIGHT, "CSD_ALIGN_RIGHT" },
    { CSD_STRETCH_HORIZONTAL, "CSD_STRETCH_HORIZONTAL" },
    { CSD_STRETCH_VERTICAL, "CSD_STRETCH_VERTICAL" },
    { CSD_UNSTRETCH_HORIZONTAL, "CSD_UNSTRETCH_HORIZONTAL" },
    { CSD_EXTEND_LEFT, "CSD_EXTEND_LEFT" },
    { CSD_EXTEND_RIGHT, "CSD_EXTEND_RIGHT" },
    { CSD_STORE_LEFT_CORNER, "CSD_STORE_LEFT_CORNER" },
    { CSD_STORE_RIGHT_CORNER, "CSD_STORE_RIGHT_CORNER" },
    { CSD_SKIP, "CSD_SKIP" },
    { CSD_OFFSET_SCALE_LEFT, "CSD_OFFSET_SCALE_LEFT" },
    { CSD_OFFSET_SCALE_RIGHT, "CSD_OFFSET_SCALE_RIGHT" },
    { CSD_REPEAT_LEFT, "CSD_REPEAT_LEFT" },
    { CSD_REPEAT_RIGHT, "CSD_REPEAT_RIGHT" },
    { CSD_REPEAT_UP, "CSD_REPEAT_UP" },
    { CSD_REPEAT_DOWN, "CSD_REPEAT_DOWN" },
    { CSD_REPEAT_FLIP_HORIZONTAL, "CSD_REPEAT_FLIP_HORIZONTAL" },
    { CSD_REPEAT_FLIP_VERTICAL, "CSD_REPEAT_FLIP_VERTICAL" },
    { CSD_REPEAT_EXTEND, "CSD_REPEAT_EXTEND" },
    { CSD_POSITION_MODIFIER, "CSD_POSITION_MODIFIER" },
    { CSD_UV_MODIFIER, "CSD_UV_MODIFIER" },
    { CSD_COLOR_MODIFIER, "CSD_COLOR_MODIFIER" },
    { CSD_REPEAT_POSITION_MODIFIER, "CSD_REPEAT_POSITION_MODIFIER" },
    { CSD_REPEAT_UV_MODIFIER, "CSD_REPEAT_UV_MODIFIER" },
    { CSD_REPEAT_COLOR_MODIFIER, "CSD_REPEAT_COLOR_MODIFIER" },
    { CSD_BLACK_BAR, "CSD_BLACK_BAR" },
    { CSD_PROHIBIT_BLACK_BAR, "CSD_PROHIBIT_BLACK_BAR" },
    { CSD_CORNER_EXTRACT, "CSD_CORNER_EXTRACT" },
    { CSD_RADARMAP, "CSD_RADARMAP" },
    { CSD_POD_BASE, "CSD_POD_BASE" },
    { CSD_POD_CLONE, "CSD_POD_CLONE" },
    { CSD_MULTIPLAYER, "CSD_MULTIPLAYER" },
    { CSD_CHEVRON, "CSD_CHEVRON" },
    { CSD_SCENE_DISABLE_MOTION, "CSD_SCENE_DISABLE_MOTION" },
    { CSD_EVENT, "CSD_EVENT" },
    { CSD_CRI_LOGO, "CSD_CRI_LOGO" },
    { CSD_BUTTON_WINDOW, "CSD_BUTTON_WINDOW" },
    { CSD_CROP, "CSD_CROP" },
    { CSD_3D, "CSD_3D" },
    { CSD_ALIGN_IGNORE_PREFERENCE_H, "CSD_ALIGN_IGNORE_PREFERENCE_H" },
    { CSD_ALIGN_IGNORE_PREFERENCE_V, "CSD_ALIGN_IGNORE_PREFERENCE_V" },
    { CSD_LIFE_COUNTER, "CSD_LIFE_COUNTER" }
};

static uint64_t ReadFlags(const nlohmann::json& in_rReader, const std::string_view in_key)
{
    uint64_t result{};

    if (!in_rReader.contains(in_key) || !in_rReader[in_key].is_array())
        return result;

    for (const auto& item : in_rReader[in_key])
    {
        const auto flagName = item.get<std::string_view>();

        for (const auto& [rFlagValue, rFlagName] : g_CsdFlagNames)
        {
            if (flagName == rFlagName)
            {
                result |= rFlagValue;
                break;
            }
        }
    }

    return result;
}

static bool WriteFlags(nlohmann::json& in_rWriter, const std::string_view in_key, const uint64_t in_flags)
{
    std::vector<std::string> flagNames{};

    for (const auto& [rFlagValue, rFlagName] : g_CsdFlagNames)
    {
        if ((in_flags & rFlagValue) != 0)
            flagNames.push_back(rFlagName);
    }

    in_rWriter[in_key] = flagNames;

    return true;
}

static bool ReadRGBA8(const nlohmann::json& in_rReader, const std::string_view in_key, Sonicteam::SoX::RGBA8& io_rColor)
{
    if (!in_rReader.contains(in_key))
        return false;

    if (in_rReader[in_key].contains("R"))
        io_rColor.Data[0] = in_rReader[in_key]["R"];

    if (in_rReader[in_key].contains("G"))
        io_rColor.Data[1] = in_rReader[in_key]["G"];

    if (in_rReader[in_key].contains("B"))
        io_rColor.Data[2] = in_rReader[in_key]["B"];

    if (in_rReader[in_key].contains("A"))
        io_rColor.Data[3] = in_rReader[in_key]["A"];

    return true;
}

static bool WriteRGBA8(nlohmann::json& in_rWriter, const std::string_view in_key, const Sonicteam::SoX::RGBA8& in_rColor)
{
    in_rWriter[in_key] =
    {
        { "R", in_rColor.Data[0].get() },
        { "G", in_rColor.Data[1].get() },
        { "B", in_rColor.Data[2].get() },
        { "A", in_rColor.Data[3].get() }
    };

    return true;
}

bool CsdVector2::Read(const nlohmann::json& in_rReader)
{
    if (in_rReader.contains("X"))
        X = in_rReader["X"];

    if (in_rReader.contains("Y"))
        Y = in_rReader["Y"];

    return true;
}

bool CsdVector2::Write(nlohmann::json& in_rWriter)
{
    if (X)
        in_rWriter["X"] = X;

    if (Y)
        in_rWriter["Y"] = Y;

    return true;
}

bool CsdVertexModifier::Read(const nlohmann::json& in_rReader)
{
    if (in_rReader.contains("X"))
        X = in_rReader["X"];

    if (in_rReader.contains("Y"))
        Y = in_rReader["Y"];

    ReadRGBA8(in_rReader, "Color", Color);

    if (in_rReader.contains("U"))
        U = in_rReader["U"];

    if (in_rReader.contains("V"))
        V = in_rReader["V"];

    return true;
}

bool CsdVertexModifier::Write(nlohmann::json& in_rWriter)
{
    if (X)
        in_rWriter["X"] = X;

    if (Y)
        in_rWriter["Y"] = Y;

    if (Color != Sonicteam::SoX::RGBA8{ { 0, 0, 0, 255 } })
        WriteRGBA8(in_rWriter, "Color", Color);

    if (U)
        in_rWriter["U"] = U;

    if (V)
        in_rWriter["V"] = V;

    return true;
}

bool CsdVertexModifierGroup::Read(const nlohmann::json& in_rReader)
{
    if (in_rReader.contains("TL"))
        TL.Read(in_rReader["TL"]);

    if (in_rReader.contains("BL"))
        BL.Read(in_rReader["BL"]);

    if (in_rReader.contains("TR"))
        TR.Read(in_rReader["TR"]);

    if (in_rReader.contains("BR"))
        BR.Read(in_rReader["BR"]);

    if (in_rReader.contains("Offset"))
        Offset.Read(in_rReader["Offset"]);

    if (in_rReader.contains("ExtendBias"))
        ExtendBias.Read(in_rReader["ExtendBias"]);

    return true;
}

bool CsdVertexModifierGroup::Write(nlohmann::json& in_rWriter)
{
    if (!TL.IsDefault())
        TL.Write(in_rWriter["TL"]);

    if (!BL.IsDefault())
        BL.Write(in_rWriter["BL"]);

    if (!TR.IsDefault())
        TR.Write(in_rWriter["TR"]);

    if (!BR.IsDefault())
        BR.Write(in_rWriter["BR"]);

    if (!Offset.IsZero())
        Offset.Write(in_rWriter["Offset"]);

    if (ExtendBias != CsdVector2(1.0f, 1.0f))
        ExtendBias.Write(in_rWriter["ExtendBias"]);

    return true;
}

bool CsdAlignmentModifier::Read(const nlohmann::json& in_rReader)
{
    Mask = ReadFlags(in_rReader, "Mask");

    if (in_rReader.contains("Base"))
        Base.Read(in_rReader["Base"]);

    if (in_rReader.contains("Repeat"))
        Repeat.Read(in_rReader["Repeat"]);

    return true;
}

bool CsdAlignmentModifier::Write(nlohmann::json& in_rWriter)
{
    if (Mask)
        WriteFlags(in_rWriter, "Mask", Mask);

    if (!Base.IsDefault())
        Base.Write(in_rWriter["Base"]);

    if (!Repeat.IsDefault())
        Repeat.Write(in_rWriter["Repeat"]);

    return true;
}

bool CsdAlignmentModifierGroup::Read(const nlohmann::json& in_rReader)
{
    Mask = ReadFlags(in_rReader, "Mask");

    if (in_rReader.contains("Edge"))
        Edge.Read(in_rReader["Edge"]);

    if (in_rReader.contains("Center"))
        Center.Read(in_rReader["Center"]);

    return true;
}

bool CsdAlignmentModifierGroup::Write(nlohmann::json& in_rWriter)
{
    if (Mask)
        WriteFlags(in_rWriter, "Mask", Mask);

    if (!Edge.IsDefault())
        Edge.Write(in_rWriter["Edge"]);

    if (!Center.IsDefault())
        Center.Write(in_rWriter["Center"]);

    return true;
}

bool CsdModifier::Read(const nlohmann::json& in_rReader)
{
    Flags = ReadFlags(in_rReader, "Flags");

    if (in_rReader.contains("Ultrawide"))
        Ultrawide.Read(in_rReader["Ultrawide"]);

    if (in_rReader.contains("Narrow"))
        Narrow.Read(in_rReader["Narrow"]);

    if (in_rReader.contains("CornerMax"))
        CornerMax = in_rReader["CornerMax"];

    if (in_rReader.contains("CornerIndex"))
        CornerIndex = in_rReader["CornerIndex"];

    return true;
}

bool CsdModifier::Write(nlohmann::json& in_rWriter)
{
    WriteFlags(in_rWriter, "Flags", Flags);

    Ultrawide.Write(in_rWriter["Ultrawide"]);
    Narrow.Write(in_rWriter["Narrow"]);

    if (CornerMax)
        in_rWriter["CornerMax"] = CornerMax;

    if (CornerIndex)
        in_rWriter["CornerIndex"] = CornerIndex;

    return true;
}

uint64_t CsdModifier::GetFlags() const
{
    auto result = Flags;

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
    {
        auto& rAlignment = Config::UIAlignmentMode == EUIAlignmentMode::Edge
            ? Ultrawide.Edge
            : Ultrawide.Center;

        // Remove all masked flags if the aspect ratio is above 16:9.
        result &= ~((Ultrawide.Mask | rAlignment.Mask) & ~CSD_SPECIAL_FLAGS);
    }
    else if (g_aspectRatio < WIDE_ASPECT_RATIO)
    {
        auto& rAlignment = Config::UIAlignmentMode == EUIAlignmentMode::Edge
            ? Narrow.Edge
            : Narrow.Center;

        // Remove all masked flags if the aspect ratio is below 16:9.
        result &= ~((Narrow.Mask | rAlignment.Mask) & ~CSD_SPECIAL_FLAGS);
    }
    else
    {
        // Remove all flags if the aspect ratio is 16:9.
        result &= ~(result & ~CSD_SPECIAL_FLAGS);

        return result;
    }

    if ((result & CSD_CHEVRON) != 0)
    {
        // Don't repeat main menu arrows at 16:9 or narrower.
        if (g_aspectRatio <= WIDE_ASPECT_RATIO)
            result &= ~(CSD_REPEAT_LEFT | CSD_REPEAT_RIGHT);
    }
    else if (Config::UIAlignmentMode == EUIAlignmentMode::Centre || BlackBar::IsVisible())
    {
        if (g_aspectRatio > WIDE_ASPECT_RATIO && (result & CSD_ALIGN_IGNORE_PREFERENCE_H) == 0)
        {
            // Remove horizontal alignments at wide aspect ratios.
            result &= ~(CSD_ALIGN_LEFT | CSD_ALIGN_RIGHT);
        }
        else if (g_aspectRatio < WIDE_ASPECT_RATIO && (result & CSD_ALIGN_IGNORE_PREFERENCE_V) == 0)
        {
            // Remove vertical alignments at narrow aspect ratios.
            result &= ~(CSD_ALIGN_TOP | CSD_ALIGN_BOTTOM);
        }

        // Don't adjust CRI logo at 16:9 or ultrawide, or when using centre alignment.
        if (g_aspectRatio >= WIDE_ASPECT_RATIO || Config::UIAlignmentMode == EUIAlignmentMode::Centre)
            result &= ~CSD_CRI_LOGO;
    }

    return result;
}

std::string CsdModifier::ToString()
{
    nlohmann::json json{};

    Write(json);

    return json.dump(4);
}
