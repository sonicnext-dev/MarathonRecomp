#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Xenon/DeviceXenon.h>
#include <d3dxb.h>

namespace Sonicteam
{
    class MyGraphicsDevice : public SoX::Graphics::Xenon::DeviceXenon
    {
    public:
        // Info : sub_8289CF60(this, CreationDeviceLocal*)
        struct CreationDeviceLocal
        {
            xpointer<void> pDevice;
            D3DXBSURFACE_PARAMETERS SurfaceParamsA;
            D3DXBSURFACE_PARAMETERS SurfaceParamsB;
            D3DXBSURFACE_PARAMETERS SurfaceParamsC;
            xpointer<void> pColorTile2x;
            xpointer<void> pDepthTile2x;
            xpointer<void> pColorTile4x;
            xpointer<void> pDepthTile4x;
            be<uint32_t> BackBufferWidth;
            be<uint32_t> BackBufferHeight;
            xpointer<uint8_t> pAtgFontFile;
        };

        MARATHON_INSERT_PADDING(0x180); 
    };

    MARATHON_ASSERT_SIZEOF(MyGraphicsDevice, 0x30C);
}
