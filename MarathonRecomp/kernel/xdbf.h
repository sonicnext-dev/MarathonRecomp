#pragma once

#include <gpu/video.h>
#include <xdbf_wrapper.h>

extern XDBFWrapper g_xdbfWrapper;
extern std::unordered_map<uint16_t, GuestTexture*> g_xdbfTextureCache;

namespace xdbf
{
    inline std::string& FixInvalidSequences(std::string& str)
    {
        static std::array<std::string_view, 1> s_invalidSequences =
        {
            "\n"
        };

        static std::array<std::string_view, 1> s_replaceSequences =
        {
            " "
        };

        for (int i = 0; i < s_invalidSequences.size(); i++)
        {
            size_t pos = 0;

            auto& invalidSeq = s_invalidSequences[i];
            auto& replaceSeq = s_replaceSequences[i];

            while ((pos = str.find(s_invalidSequences[i], pos)) != std::string::npos)
            {
                str = str.replace(pos, invalidSeq.length(), replaceSeq);

                pos += replaceSeq.length();
            }
        }

        return str;
    }
}
