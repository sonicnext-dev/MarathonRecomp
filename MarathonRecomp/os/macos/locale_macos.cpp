#include <os/locale.h>

std::string os::locale::ConvertToUTF8(Encoding in_encoding, const std::string& in_rStr)
{
    std::string result{};

    switch (in_encoding)
    {
        case Encoding::ShiftJIS:
        {
            // TODO
            break;
        }
    }

    return result;
}
