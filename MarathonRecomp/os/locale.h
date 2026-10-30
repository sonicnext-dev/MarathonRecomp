#pragma once

#include <string>

namespace os::locale
{
    enum class Encoding
    {
        ShiftJIS
    };

    std::string ConvertToUTF8(Encoding in_encoding, const std::string& in_rStr);
}
