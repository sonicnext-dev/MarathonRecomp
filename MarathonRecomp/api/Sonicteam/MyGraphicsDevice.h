#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Xenon/DeviceXenon.h>
#include <gpu/video.h>

namespace Sonicteam
{

    class MyGraphicsDevice : public SoX::Graphics::Xenon::DeviceXenon
    {
    public:

        // Info : sub_8289CF60(this,CreationDeviceLocal*)
        struct CreationDeviceLocal
        {
            xpointer<GuestDevice> pGuestDevice;
            GuestSurfaceCreateParams SurfaceParamA;
            GuestSurfaceCreateParams SurfaceParamB;
            GuestSurfaceCreateParams SurfaceParamC;
            xpointer<GuestSurface> pColorTile2X;
            xpointer<GuestSurface> pDepthTile2X;
            xpointer<GuestSurface> pColorTile4X;
            xpointer<GuestSurface> pDepthTile4X;
            be<uint32_t> BackBufferWidth;
            be<uint32_t> BackBufferHeight;
            xpointer<char> AtgFontFile;
        };

        MARATHON_INSERT_PADDING(0x180); 
    };

    MARATHON_ASSERT_SIZEOF(MyGraphicsDevice, 0x350);
}
