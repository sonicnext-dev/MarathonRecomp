#include <os/locale.h>

std::string os::locale::ConvertToUTF8(Encoding in_encoding, const std::string& in_rStr)
{
    std::string result{};

    switch (in_encoding)
    {
        case Encoding::ShiftJIS:
        {
            const auto lengthU16 = MultiByteToWideChar(932, 0, in_rStr.c_str(), in_rStr.size(), NULL, 0);

            if (lengthU16 <= 0)
                break;

            auto wide = std::wstring(lengthU16, 0);
            MultiByteToWideChar(932, 0, in_rStr.c_str(), in_rStr.size(), &wide[0], lengthU16);

            const auto lengthU8 = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), lengthU16, NULL, 0, NULL, NULL);

            if (lengthU8 <= 0)
                break;

            result = std::string(lengthU8, 0);
            WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), lengthU16, &result[0], lengthU8, NULL, NULL);

            break;
        }
    }

    return result;
}
