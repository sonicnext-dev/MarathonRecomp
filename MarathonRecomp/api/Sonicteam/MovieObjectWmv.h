#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/MyTexture.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class MovieObjectWmv : public SoX::RefCountObject
    {
    public:
        bool m_UseCustomDimensions;
        be<float> m_Left;
        be<float> m_Top;
        be<float> m_Right;
        be<float> m_Bottom;
        MARATHON_INSERT_PADDING(4);
        stdx::string m_FilePath;
        MARATHON_INSERT_PADDING(8);
        bool m_RenderMovie;
        MARATHON_INSERT_PADDING(0x38);
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        be<uint32_t> m_Field84;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<MyTexture> m_pTexturesYUV[4 * 3];
        MARATHON_INSERT_PADDING(8);
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<void> m_pTechnique;
        stdx::string m_Language;
    };
}
