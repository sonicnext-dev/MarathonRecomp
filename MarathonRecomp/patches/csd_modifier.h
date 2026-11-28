#pragma once

#include <api/Marathon.h>
#include <utils/i_json_serializable.h>

#define MAKE_BITFLAG64(bit) 1ULL << bit

enum CsdFlags : uint64_t
{
    CSD_ALIGN_CENTER = 0,
    CSD_ALIGN_TOP = MAKE_BITFLAG64(0),
    CSD_ALIGN_LEFT = MAKE_BITFLAG64(1),
    CSD_ALIGN_BOTTOM = MAKE_BITFLAG64(2),
    CSD_ALIGN_RIGHT = MAKE_BITFLAG64(3),
    CSD_ALIGN_TOP_LEFT = CSD_ALIGN_TOP | CSD_ALIGN_LEFT,
    CSD_ALIGN_TOP_RIGHT = CSD_ALIGN_TOP | CSD_ALIGN_RIGHT,
    CSD_ALIGN_BOTTOM_LEFT = CSD_ALIGN_BOTTOM | CSD_ALIGN_LEFT,
    CSD_ALIGN_BOTTOM_RIGHT = CSD_ALIGN_BOTTOM | CSD_ALIGN_RIGHT,

    CSD_STRETCH_HORIZONTAL = MAKE_BITFLAG64(4),
    CSD_STRETCH_VERTICAL = MAKE_BITFLAG64(5),
    CSD_STRETCH = CSD_STRETCH_HORIZONTAL | CSD_STRETCH_VERTICAL,

    CSD_EXTEND_LEFT = MAKE_BITFLAG64(6),
    CSD_EXTEND_RIGHT = MAKE_BITFLAG64(7),
    CSD_EXTEND = CSD_EXTEND_LEFT | CSD_EXTEND_RIGHT,

    CSD_STORE_LEFT_CORNER = MAKE_BITFLAG64(8),
    CSD_STORE_RIGHT_CORNER = MAKE_BITFLAG64(9),

    CSD_SKIP = MAKE_BITFLAG64(10),

    CSD_OFFSET_SCALE_LEFT = MAKE_BITFLAG64(11),
    CSD_OFFSET_SCALE_RIGHT = MAKE_BITFLAG64(12),

    CSD_POSITION_MODIFIER = MAKE_BITFLAG64(13),
    CSD_UV_MODIFIER = MAKE_BITFLAG64(14),
    CSD_COLOR_MODIFIER = MAKE_BITFLAG64(15),

    // TODO: these should be bit flags inside CsdVertexModifierGroup.
    CSD_REPEAT_LEFT = MAKE_BITFLAG64(16),
    CSD_REPEAT_RIGHT = MAKE_BITFLAG64(17),
    CSD_REPEAT_UP = MAKE_BITFLAG64(18),
    CSD_REPEAT_DOWN = MAKE_BITFLAG64(19),

    // TODO: these could be set up to work without repeat too.
    CSD_REPEAT_FLIP_HORIZONTAL = MAKE_BITFLAG64(20),
    CSD_REPEAT_FLIP_VERTICAL = MAKE_BITFLAG64(21),

    // TODO: this could be a boolean inside CsdVertexModifierGroup.
    CSD_REPEAT_EXTEND = MAKE_BITFLAG64(22),

    // TODO: these could be removed and each CsdVertexModifierGroup have a set of flags instead.
    CSD_REPEAT_POSITION_MODIFIER = MAKE_BITFLAG64(23),
    CSD_REPEAT_UV_MODIFIER = MAKE_BITFLAG64(24),
    CSD_REPEAT_COLOR_MODIFIER = MAKE_BITFLAG64(25),

    CSD_CORNER_EXTRACT = MAKE_BITFLAG64(26),

    CSD_RADARMAP = MAKE_BITFLAG64(27),

    CSD_POD_BASE = MAKE_BITFLAG64(28),
    CSD_POD_CLONE = MAKE_BITFLAG64(29),

    CSD_MULTIPLAYER = MAKE_BITFLAG64(30),

    CSD_CHEVRON = MAKE_BITFLAG64(31),

    CSD_SCENE_DISABLE_MOTION = MAKE_BITFLAG64(32),

    CSD_EVENT = MAKE_BITFLAG64(33),

    CSD_CRI_LOGO = MAKE_BITFLAG64(34),

    CSD_BUTTON_WINDOW = MAKE_BITFLAG64(35),

    CSD_CROP_TO_FIT = MAKE_BITFLAG64(36),
    CSD_CROP_TO_WIDESCREEN = MAKE_BITFLAG64(37),

    CSD_3D = MAKE_BITFLAG64(38),

    CSD_ALIGN_IGNORE_PREFERENCE_H = MAKE_BITFLAG64(39),
    CSD_ALIGN_IGNORE_PREFERENCE_V = MAKE_BITFLAG64(40),
    CSD_ALIGN_IGNORE_PREFERENCE = CSD_ALIGN_IGNORE_PREFERENCE_H | CSD_ALIGN_IGNORE_PREFERENCE_V,

    CSD_LIFE_COUNTER = MAKE_BITFLAG64(41),

    CSD_SPECIAL_FLAGS = CSD_RADARMAP | CSD_BUTTON_WINDOW | CSD_LIFE_COUNTER
};

extern std::map<uint64_t, const char*> g_CsdFlagNames;

enum CsdVertexID
{
    CSD_TL,
    CSD_BL,
    CSD_TR,
    CSD_BR
};

class CsdVector2 : public IJsonSerializable
{
public:
    float X{};
    float Y{};

    CsdVector2() = default;
    CsdVector2(float in_x, float in_y) : X(in_x), Y(in_y) {}

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    bool IsZero() const
    {
        return X == 0 && Y == 0;
    }

    bool operator==(const CsdVector2& in_rOther) const
    {
        return X == in_rOther.X && Y == in_rOther.Y;
    }
};

class CsdOffsetTransform : public IJsonSerializable
{
public:
    float X{};
    float Y{};
    float U{};
    float V{};

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    bool IsDefault() const
    {
        return X == 0 && Y == 0 && U == 0 && V == 0;
    }

    bool operator==(const CsdOffsetTransform& in_rOther) const
    {
        return X == in_rOther.X &&
               Y == in_rOther.Y &&
               U == in_rOther.U &&
               V == in_rOther.V;
    }
};

class CsdVertexModifier : public CsdOffsetTransform
{
public:
    enum class ColorMode
    {
        Multiply,
        Set
    };

    Sonicteam::SoX::RGBAF32 Color{};
    ColorMode ColorMode{};

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    void ApplyColorModifier(Chao::CSD::Vertex* in_pVertex) const
    {
        switch (ColorMode)
        {
            case ColorMode::Multiply:
                in_pVertex->Color *= Color;
                break;

            case ColorMode::Set:
                in_pVertex->Color.SetChannelNormalized(Sonicteam::SoX::ColorChannel_R, Color.Data[0]);
                in_pVertex->Color.SetChannelNormalized(Sonicteam::SoX::ColorChannel_G, Color.Data[1]);
                in_pVertex->Color.SetChannelNormalized(Sonicteam::SoX::ColorChannel_B, Color.Data[2]);
                in_pVertex->Color.SetChannelNormalized(Sonicteam::SoX::ColorChannel_A, Color.Data[3]);
                break;
        }
    }

    bool IsDefault() const
    {
        return CsdOffsetTransform::IsDefault() &&
               Color == Sonicteam::SoX::RGBAF32() &&
               ColorMode == ColorMode::Multiply;
    }

    bool operator==(const CsdVertexModifier& in_rOther) const
    {
        return CsdOffsetTransform::operator==(in_rOther) &&
               Color == in_rOther.Color &&
               ColorMode == in_rOther.ColorMode;
    }
};

class CsdExtendTransform : public IJsonSerializable
{
public:
    CsdVector2 TopLeft{ 1.0f, 1.0f };
    CsdVector2 BottomRight{ 1.0f, 1.0f };

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    bool IsDefault() const
    {
        return TopLeft == CsdVector2(1.0f, 1.0f) &&
               BottomRight == CsdVector2(1.0f, 1.0f);
    }

    bool operator==(const CsdExtendTransform& in_rOther) const
    {
        return TopLeft == in_rOther.TopLeft &&
               BottomRight == in_rOther.BottomRight;
    }
};

class CsdVertexModifierGroup : public IJsonSerializable
{
public:
    uint64_t Flags{};

    CsdVertexModifier TL{};
    CsdVertexModifier BL{};
    CsdVertexModifier TR{};
    CsdVertexModifier BR{};

    CsdOffsetTransform Offset{};
    CsdExtendTransform Extend{};

    std::vector<CsdVertexModifierGroup> Slices{};

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    bool IsDefault() const
    {
        return Flags == 0 &&
               TL.IsDefault() &&
               BL.IsDefault() &&
               TR.IsDefault() &&
               BR.IsDefault() &&
               Offset.IsDefault() &&
               Extend.IsDefault() &&
               Slices.empty();
    }

    CsdVertexModifier& GetVertexModifier(const CsdVertexID in_id)
    {
        switch (in_id)
        {
            case CSD_TL: return TL;
            case CSD_BL: return BL;
            case CSD_TR: return TR;
            case CSD_BR: return BR;
        }

        return TL;
    }

    size_t GetTotalSliceCount() const
    {
        size_t result = Slices.size();

        for (const auto& rSlice : Slices)
            result += rSlice.GetTotalSliceCount();

        return result;
    }

    bool operator==(const CsdVertexModifierGroup& in_rOther) const
    {
        return Flags == in_rOther.Flags &&
               TL == in_rOther.TL &&
               BL == in_rOther.BL &&
               TR == in_rOther.TR &&
               BR == in_rOther.BR &&
               Offset == in_rOther.Offset &&
               Extend == in_rOther.Extend &&
               Slices == in_rOther.Slices;
    }
};

class CsdAlignmentModifier : public IJsonSerializable
{
public:
    uint64_t Mask{};

    CsdVertexModifierGroup Base{};
    CsdVertexModifierGroup Repeat{};

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    bool IsDefault() const
    {
        return Mask == 0 && Base.IsDefault() && Repeat.IsDefault();
    }

    bool operator==(const CsdAlignmentModifier& in_rOther) const
    {
        return Mask == in_rOther.Mask && Base == in_rOther.Base && Repeat == in_rOther.Repeat;
    }
};

class CsdAlignmentModifierGroup : public IJsonSerializable
{
public:
    uint64_t Mask{};

    CsdAlignmentModifier Edge{};
    CsdAlignmentModifier Center{};

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    bool IsDefault() const
    {
        return Mask == 0 && Edge.IsDefault() && Center.IsDefault();
    }

    bool operator==(const CsdAlignmentModifierGroup& in_rOther) const
    {
        return Mask == in_rOther.Mask && Edge == in_rOther.Edge && Center == in_rOther.Center;
    }
};

class CsdModifier : public IJsonSerializable
{
public:
    std::string Path{};

    uint64_t Flags{};

    CsdAlignmentModifierGroup Ultrawide{};
    CsdAlignmentModifierGroup Narrow{};

    float CornerMax{};
    int CornerIndex{};

    virtual bool Read(const nlohmann::json& in_rReader) override;
    virtual bool Write(nlohmann::json& in_rWriter) override;

    size_t GetSize() const
    {
        size_t result = sizeof(CsdModifier);

        const auto addAlignmentModifierGroupSize = [&](const CsdAlignmentModifierGroup& in_rModifier)
        {
            result += in_rModifier.Edge.Base.GetTotalSliceCount() * sizeof(CsdVertexModifierGroup);
            result += in_rModifier.Edge.Repeat.GetTotalSliceCount() * sizeof(CsdVertexModifierGroup);
            result += in_rModifier.Center.Base.GetTotalSliceCount() * sizeof(CsdVertexModifierGroup);
            result += in_rModifier.Center.Repeat.GetTotalSliceCount() * sizeof(CsdVertexModifierGroup);
        };

        addAlignmentModifierGroupSize(Ultrawide);
        addAlignmentModifierGroupSize(Narrow);

        return result;
    }

    uint64_t GetFlags() const;

    std::string ToString();
};

#undef MAKE_BITFLAG64
