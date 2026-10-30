#include <os/locale.h>
#include <CoreFoundation/CoreFoundation.h>

std::string os::locale::ConvertToUTF8(Encoding in_encoding, const std::string& in_rStr)
{
    CFStringEncoding cfEncoding;

    switch (in_encoding)
    {
        case Encoding::ShiftJIS:
            cfEncoding = kCFStringEncodingDOSJapanese;
            break;

        default:
            assert(false && "Unimplemented.");
            return "";
    }

    CFDataRef cfData = CFDataCreateWithBytesNoCopy(nullptr, reinterpret_cast<const UInt8*>(in_rStr.data()),
        static_cast<CFIndex>(in_rStr.size()), kCFAllocatorNull);
    assert(cfData && "CFDataCreateWithBytesNoCopy failed");

    CFStringRef cfStr = CFStringCreateFromExternalRepresentation(nullptr, cfData, cfEncoding);
    CFRelease(cfData);
    assert(cfStr && "CFStringCreateFromExternalRepresentation failed");

    CFDataRef cfConvertedData = CFStringCreateExternalRepresentation(nullptr, cfStr, kCFStringEncodingUTF8, 0);
    CFRelease(cfStr);
    assert(cfConvertedData && "CFStringCreateExternalRepresentation failed");

    std::string result(reinterpret_cast<const char*>(CFDataGetBytePtr(cfConvertedData)), CFDataGetLength(cfConvertedData));
    CFRelease(cfConvertedData);

    return result;
}
