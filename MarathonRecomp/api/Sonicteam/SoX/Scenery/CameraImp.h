#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Scenery
{
    class CameraImp : public Camera
    {
    public:
        class frameObserver : public Graphics::FrameObserver
        {
        public:
            xpointer<CameraImp> m_pCameraImp;
            be<uint32_t> m_Field18;
            MARATHON_INSERT_PADDING(0x14);
            SoX::Math::Matrix4x4 m_Field30;
            SoX::Math::Matrix4x4 m_Field70;
            SoX::Math::Quaternion m_FieldB0;
            SoX::Math::Vector m_FieldC0;
            MARATHON_INSERT_PADDING(0x20);
            xpointer<void> m_pWorldIntersectionStandard;
            MARATHON_INSERT_PADDING(0x10);
            xpointer<MyTransforms> m_pMyTransforms;
            MARATHON_INSERT_PADDING(8);
        };

        stdx::string m_Name;
        be<float> m_FOV;
        be<float> m_AspectRatioWidth;
        be<float> m_AspectRatioHeight;
        be<float> m_Near;
        be<float> m_Far;
        xpointer<void> m_pWorldImp;
        MARATHON_INSERT_PADDING(0x18);
        SoX::Math::Matrix4x4 m_ViewMatrix;
        SoX::Math::Matrix4x4 m_Field90;
        SoX::Math::Matrix4x4 m_FieldD0;
        frameObserver m_FrameObserver;
    };
}
