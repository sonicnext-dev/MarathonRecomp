#pragma once

#include <concepts>
#include <limits>
#include <optional>
#include <Marathon.inl>

#define MAKE_COLOR_CHANNEL(COLOR)                                               \
    TChannelType COLOR(std::optional<TChannelType> in_##COLOR = std::nullopt)   \
    {                                                                           \
        if (!HasColorChannel<Format>(ColorChannel_##COLOR))                     \
            return std::numeric_limits<TChannelType>::max();                    \
                                                                                \
        auto& rData = Data[GetColorChannelIndex<Format>(ColorChannel_##COLOR)]; \
                                                                                \
        if (in_##COLOR)                                                         \
            rData = *in_##COLOR;                                                \
                                                                                \
        return rData;                                                           \
    }

#define MAKE_MATH_OPERATOR(OPERATOR)                                                                   \
    template <ColorNumeric TOtherChannelType, ColorFormat OtherFormat, size_t OtherFields>             \
    Color operator OPERATOR(const Color<TOtherChannelType, OtherFormat, OtherFields>& in_rOther) const \
    {                                                                                                  \
        Color result = *this;                                                                          \
        result OPERATOR##= in_rOther;                                                                  \
        return result;                                                                                 \
    }                                                                                                  \
                                                                                                       \
    template <ColorNumeric TOtherChannelType, ColorFormat OtherFormat, size_t OtherFields>             \
    Color& operator OPERATOR##=(const Color<TOtherChannelType, OtherFormat, OtherFields>& in_rOther)   \
    {                                                                                                  \
        constexpr ColorChannel channels[] =                                                            \
        {                                                                                              \
            ColorChannel_R, ColorChannel_G, ColorChannel_B, ColorChannel_A                             \
        };                                                                                             \
                                                                                                       \
        for (const auto channel : channels)                                                            \
        {                                                                                              \
            if (!HasColorChannel<Format>(channel))                                                     \
                continue;                                                                              \
                                                                                                       \
            const auto left = GetChannelNormalized(channel);                                           \
            const auto right = in_rOther.GetChannelNormalized(channel);                                \
                                                                                                       \
            SetChannelNormalized(channel, left OPERATOR right);                                        \
        }                                                                                              \
                                                                                                       \
        return *this;                                                                                  \
    }

namespace Sonicteam::SoX
{
    template <typename T>
    concept ColorNumeric = std::integral<T> || std::floating_point<T>;

    enum ColorFormat
    {
        ColorFormat_RGB,
        ColorFormat_RGBA,
        ColorFormat_ARGB
    };
    
    enum ColorChannel
    {
        ColorChannel_R,
        ColorChannel_G,
        ColorChannel_B,
        ColorChannel_A
    };

    template <ColorFormat Format>
    static inline constexpr int GetColorChannelCount()
    {
        switch (Format)
        {
            case ColorFormat_RGB:
                return 3;

            default:
                return 4;
        }
    }

    template <ColorFormat Format>
    static inline constexpr int GetColorChannelIndex(ColorChannel in_channel)
    {
        int result = in_channel;

        if constexpr (Format == ColorFormat_ARGB)
        {
            if (result == ColorChannel_A)
            {
                result = ColorChannel_R;
            }
            else
            {
                result++;
            }
        }

        return result;
    }

    template <ColorFormat Format>
    static inline constexpr bool HasColorChannel(ColorChannel in_channel)
    {
        if constexpr (Format == ColorFormat_RGB)
        {
            if (in_channel == ColorChannel_A)
                return false;
        }

        return true;
    }

    template <ColorNumeric TChannelType, ColorFormat Format, size_t Fields = GetColorChannelCount<Format>()>
    struct Color
    {
        be<TChannelType> Data[Fields]{};

        Color()
        {
            for (size_t i = 0; i < Fields; i++)
                Data[i] = GetChannelMax();
        }

        Color(const TChannelType in_r, const TChannelType in_g, const TChannelType in_b,
              const TChannelType in_a = GetChannelMax())
        {
            R(in_r);
            G(in_g);
            B(in_b);
            A(in_a);
        }

        Color(const Color& in_rColor)
        {
            if (sizeof(Data) != sizeof(in_rColor.Data))
                return;

            memcpy(Data, in_rColor.Data, sizeof(Data));
        }

        static constexpr TChannelType GetChannelMax()
        {
            if constexpr (std::is_floating_point_v<TChannelType>)
            {
                return TChannelType(1.0);
            }
            else
            {
                return std::numeric_limits<TChannelType>::max();
            }
        }

        TChannelType GetChannel(const ColorChannel in_channel) const
        {
            if (!HasColorChannel<Format>(in_channel))
                return GetChannelMax();

            return Data[GetColorChannelIndex<Format>(in_channel)];
        }

        bool SetChannel(const ColorChannel in_channel, const TChannelType in_value)
        {
            if (!HasColorChannel<Format>(in_channel))
                return false;

            Data[GetColorChannelIndex<Format>(in_channel)] = in_value;

            return true;
        }

        float GetChannelNormalized(const ColorChannel in_channel) const
        {
            const auto channel = GetChannel(in_channel);

            if constexpr (std::is_floating_point_v<TChannelType>)
            {
                return static_cast<float>(channel);
            }
            else
            {
                return static_cast<float>(channel) / static_cast<float>(std::numeric_limits<TChannelType>::max());
            }
        }

        void SetChannelNormalized(const ColorChannel in_channel, float in_value)
        {
            in_value = std::clamp(in_value, 0.0f, 1.0f);

            if constexpr (std::is_floating_point_v<TChannelType>)
            {
                SetChannel(in_channel, static_cast<TChannelType>(in_value));
            }
            else
            {
                SetChannel(in_channel, static_cast<TChannelType>(std::round(in_value * static_cast<float>(std::numeric_limits<TChannelType>::max()))));
            }
        }

        MAKE_COLOR_CHANNEL(R);
        MAKE_COLOR_CHANNEL(G);
        MAKE_COLOR_CHANNEL(B);
        MAKE_COLOR_CHANNEL(A);

        void FromU32(const uint32_t in_color)
        {
            R((in_color & 0xFF000000) >> 24);
            G((in_color & 0x00FF0000) >> 16);
            B((in_color & 0x0000FF00) >> 8);
            A((in_color & 0x000000FF));
        }

        MAKE_MATH_OPERATOR(/);
        MAKE_MATH_OPERATOR(*);
        MAKE_MATH_OPERATOR(+);
        MAKE_MATH_OPERATOR(-);

        bool operator==(const Color& in_rOther) const
        {
            return Data[0] == in_rOther.Data[0] &&
                   Data[1] == in_rOther.Data[1] &&
                   Data[2] == in_rOther.Data[2] &&
                   Data[3] == in_rOther.Data[3];
        }
    };
    
    struct RGB8 : Color<uint8_t, ColorFormat_RGB> {};

    struct RGBF32 : Color<float, ColorFormat_RGB> {};

    struct RGBA8 : Color<uint8_t, ColorFormat_RGBA> {};

    struct RGBAF32 : Color<float, ColorFormat_RGBA> {};

    struct ARGB8 : Color<uint8_t, ColorFormat_ARGB> {};

    struct ARGBF32 : Color<float, ColorFormat_ARGB> {};
}

#undef MAKE_MATH_OPERATOR
#undef MAKE_COLOR_CHANNEL
