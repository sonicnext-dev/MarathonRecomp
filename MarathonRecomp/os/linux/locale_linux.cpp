#include <os/locale.h>
#include <iconv.h>

#define ICONV_INVALID (reinterpret_cast<iconv_t>(-1))

class IconvContext
{
public:
    IconvContext(const char* to, const char* from)
    {
        cd = iconv_open(to, from);
        assert(cd != ICONV_INVALID && "iconv_open failed");
    }

    ~IconvContext()
    {
        if (cd != ICONV_INVALID)
            iconv_close(cd);
    }

    std::string Convert(const std::string& str)
    {
        std::string result{};
        if (cd == ICONV_INVALID)
            return result;

        auto* in = const_cast<char*>(str.data());
        size_t in_remain = str.size();

        char buf[128];
        char* out = buf;
        size_t out_remain = sizeof(buf);

        while (in_remain > 0)
        {
            if (iconv(cd, &in, &in_remain, &out, &out_remain) == -1)
            {
                break;
            }
            result.append(buf, out);

            out = buf;
            out_remain = sizeof(buf);
        }

        return result;
    }

private:
    iconv_t cd = ICONV_INVALID;
};

std::string os::locale::ConvertToUTF8(Encoding in_encoding, const std::string& in_rStr)
{
    switch (in_encoding)
    {
        case Encoding::ShiftJIS:
            thread_local IconvContext context("UTF-8", "CP932");
            return context.Convert(in_rStr);
    }

    assert(false && "Unimplemented.");

    return "";
}
