#pragma once

#include <gpu/video.h>
#include <xdbf_wrapper.h>

extern XDBFWrapper g_xdbfWrapper;
extern std::unordered_map<uint16_t, GuestTexture*> g_xdbfTextureCache;

namespace xdbf
{
    inline std::string FixInvalidSequences(std::string& str)
    {
        std::string result;

        result.reserve(str.size());

        for (size_t i = 0; i < str.size(); ++i)
        {
            auto& c = str[i];

            if (c == '\r' || c == '\n')
            {
                // Remove spaces before line breaks.
                while (!result.empty() && result.back() == ' ')
                    result.pop_back();

                // Skip duplicate line breaks.
                while (i + 1 < str.size() && (str[i + 1] == '\r' || str[i + 1] == '\n'))
                    ++i;

                // Add a space if the next char isn't one.
                if (i + 1 < str.size() && str[i + 1] != ' ')
                    result += ' ';
            }
            else
            {
                result += c;
            }
        }

        return result;
    }
}
