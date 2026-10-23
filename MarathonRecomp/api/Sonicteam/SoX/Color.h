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

namespace Sonicteam::SoX
{
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

    template <std::integral TChannelType, ColorFormat Format, size_t Fields = GetColorChannelCount<Format>()>
    struct Color
    {
        TChannelType Data[Fields]{};

        Color() = default;

        Color(const TChannelType in_r, const TChannelType in_g, const TChannelType in_b,
              const TChannelType in_a = std::numeric_limits<TChannelType>::max())
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

        MAKE_COLOR_CHANNEL(R);
        MAKE_COLOR_CHANNEL(G);
        MAKE_COLOR_CHANNEL(B);
        MAKE_COLOR_CHANNEL(A);
    };
    
    struct RGB888 : Color<uint8_t, ColorFormat_RGB> {};

    struct RGBA8888 : Color<uint8_t, ColorFormat_RGBA> {};

    struct ARGB8888 : Color<uint8_t, ColorFormat_ARGB> {};
}

#undef MAKE_COLOR_CHANNEL
