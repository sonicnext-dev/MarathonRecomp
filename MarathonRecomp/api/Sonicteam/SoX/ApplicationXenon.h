#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Application.h>
#include <gpu/video.h>


namespace Sonicteam::SoX
{
    class ApplicationXenon : public Engine::Application
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
        uint64_t m_FirstTimeBase;
        uint64_t m_FirstPerfomanceFraquency;
        uint32_t m_GuestDeviceState;
        xpointer<GuestDevice> m_GuestDevice;

        //set only if device is created
        enum _GUESTMULTISAMPLE_TYPE : int32_t
        {
            GUESTMULTISAMPLE_NONE = 0x0,
            GUESTMULTISAMPLE_2_SAMPLES = 0x1,
            GUESTMULTISAMPLE_4_SAMPLES = 0x2,
            GUESTMULTISAMPLE_FORCE_DWORD = 0x7FFFFFFF,
        };

        enum _GUESTSWAPEFFECT : int32_t
        {
            GUESTSWAPEFFECT_DISCARD = 0x1,
            GUESTSWAPEFFECT_FLIP = 0x2,
            GUESTSWAPEFFECT_COPY = 0x3,
            GUESTSWAPEFFECT_FORCE_DWORD = 0x7FFFFFFF,
        };

        enum _GUESTCOLORSPACE : int32_t
        {
            GUESTCOLORSPACE_RGB = 0x0,
            GUESTCOLORSPACE_YCbCr601 = 0x1,
            GUESTCOLORSPACE_YCbCr709 = 0x2,
            GUESTCOLORSPACE_FORCE_DWORD = 0x7FFFFFFF,
        };

        struct _GUESTRING_BUFFER_PARAMETERS
        {
            be<uint32_t> Flags;
            be<uint32_t> PrimarySize;
            xpointer<void> pPrimary;
            be<uint32_t> SecondarySize;
            xpointer<void> pSecondary;
            be<uint32_t> SegmentCount;
        };

        struct _GUESTVIDEO_SCALER_PARAMETERS
        {
            GuestRect ScalerSourceRect;
            be<uint32_t> ScaledOutputWidth;
            be<uint32_t> ScaledOutputHeight;
            be<uint32_t> FilterProfile;
        };

        struct _GUESTPRESENT_PARAMETERS_
        {
            be<uint32_t> BackBufferWidth;
            be<uint32_t> BackBufferHeight;
            be<GuestFormat> BackBufferFormat;
            be<uint32_t> BackBufferCount;
            be<_GUESTMULTISAMPLE_TYPE> MultiSampleType;
            be<uint32_t> MultiSampleQuality;
            be<_GUESTSWAPEFFECT> SwapEffect;
            xpointer<void> hDeviceWindow; //HWND??
            be<int32_t> Windowed;
            be<int32_t> EnableAutoDepthStencil;
            be<GuestFormat> AutoDepthStencilFormat;
            be<uint32_t> Flags;
            be<uint32_t> FullScreen_RefreshRateInHz;
            be<uint32_t> PresentationInterval;
            be<int32_t> DisableAutoBackBuffer;
            be<int32_t> DisableAutoFrontBuffer;
            be<GuestFormat> FrontBufferFormat;
            be < _GUESTCOLORSPACE> FrontBufferColorSpace;
            _GUESTRING_BUFFER_PARAMETERS RingBufferParameters;
            _GUESTVIDEO_SCALER_PARAMETERS VideoScalerParameters;
        };

        struct GuestDeviceInfo
        {
            be<uint32_t> GuestDeviceState;
            xpointer<::GuestDevice> pGuestDevice;
            GuestSurfaceCreateParams SurfaceParamA;
            GuestSurfaceCreateParams SurfaceParamB;
            GuestSurfaceCreateParams SurfaceParamC;
            xpointer<GuestSurface> pColorTile2X;
            xpointer<GuestSurface> pDepthTile2X;
            xpointer<GuestSurface> pColorTile4X;
            xpointer<GuestSurface> pDepthTile4X;
            _GUESTPRESENT_PARAMETERS_ GuestPresentParameters;
        };

        GuestDeviceInfo m_GuestDeviceInfo;

        //TODO: check once more these (for order)
        be<uint64_t> m_DeltaTimePost;
        be<uint64_t> m_DeltaTimePre;

        bool m_IsBufferInitialized; //buffers? all initialized, created, better name pls
        MARATHON_INSERT_PADDING(7);
        xpointer<GuestTexture> m_pFrontBufferTexture;
        xpointer<GuestSurface> m_pBackBufferSurface;
        xpointer<GuestSurface> m_pDepthStencilSurface;
        xpointer<GuestSurface> m_pColorTile2X;
        xpointer<GuestSurface> m_pDepthTile2X;
        xpointer<GuestSurface> m_pColorTile4X;
        xpointer<GuestSurface> m_pDepthTile4X;

        be<uint32_t> m_ShaderVertexCount;
        be<uint32_t> m_PixelShaderVertexCount;
        be<uint32_t> m_hNotification; //handle

        MARATHON_INSERT_PADDING(0x60);

        //static ApplicationXenon* GetInstance();
    };
<<<<<<< HEAD

    MARATHON_ASSERT_SIZEOF(ApplicationXenon, 0x180);
=======
    //0x384
>>>>>>> 4bdfb37 (Graphics Api & Resize Base)
}

//#include <Sonicteam/SoX/ApplicationXenon.inl>
