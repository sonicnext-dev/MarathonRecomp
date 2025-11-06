#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameObserver.h>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Math/Quaternion.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/MyTransforms.h>
#include <stdx/string.h>

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
            Math::Matrix4x4 m_Field30;
            Math::Matrix4x4 m_Field70;
            Math::Quaternion m_FieldB0;
            Math::Vector m_FieldC0;
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
        Math::Matrix4x4 m_ViewMatrix;
        Math::Matrix4x4 m_Field90;
        Math::Matrix4x4 m_FieldD0;
        frameObserver m_FrameObserver;
    };

    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_pCameraImp, 0x14);
    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_Field18, 0x18);
    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_Field30, 0x30);
    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_Field70, 0x70);
    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_FieldB0, 0xB0);
    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_FieldC0, 0xC0);
    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_pWorldIntersectionStandard, 0xF0);
    MARATHON_ASSERT_OFFSETOF(CameraImp::frameObserver, m_pMyTransforms, 0x104);
    MARATHON_ASSERT_SIZEOF(CameraImp::frameObserver, 0x110);

    MARATHON_ASSERT_OFFSETOF(CameraImp, m_Name, 0x04);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_FOV, 0x20);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_AspectRatioWidth, 0x24);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_AspectRatioHeight, 0x28);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_Near, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_Far, 0x30);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_pWorldImp, 0x34);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_ViewMatrix, 0x50);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_Field90, 0x90);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_FieldD0, 0xD0);
    MARATHON_ASSERT_OFFSETOF(CameraImp, m_FrameObserver, 0x110);
    MARATHON_ASSERT_SIZEOF(CameraImp, 0x220);
}
