#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Scenery
{
    class Camera
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x3C);
            be<uint32_t> fpSetFar;
            be<uint32_t> fpSetFOV;
            be<uint32_t> fpSetViewMatrix;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpSetAspectRatio;
            MARATHON_INSERT_PADDING(0x1C);
            be<uint32_t> fpSetWorldIntersectionCSM;
            MARATHON_INSERT_PADDING(0x1C);
            be<uint32_t> fpUpdate;
        };

        xpointer<Vftable> m_pVftable;

        int SetFar(double in_far)
        {
            return GuestToHostFunction<int>(m_pVftable->fpSetFar.get(), this, in_far);
        }

        int SetFOV(double in_fov)
        {
            return GuestToHostFunction<int>(m_pVftable->fpSetFOV.get(), this, in_fov);
        }

        int SetAspectRatio(float in_width, float in_height)
        {
            struct AspectRatio
            {
                be<float> Width;
                be<float> Height;
            };

            const auto aspectRatio = guest_stack_var<AspectRatio>(in_width, in_height);

            return GuestToHostFunction<int>(m_pVftable->fpSetAspectRatio.get(), this, aspectRatio.get());
        }

        int Update()
        {
            struct Unknown
            {
                MARATHON_INSERT_PADDING(0x40);
            };

            guest_stack_var<Unknown> unknown;

            return GuestToHostFunction<int>(m_pVftable->fpUpdate.get(), unknown.get(), this);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Camera::Vftable, fpSetFar, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Camera::Vftable, fpSetFOV, 0x40);
    MARATHON_ASSERT_OFFSETOF(Camera::Vftable, fpSetViewMatrix, 0x44);
    MARATHON_ASSERT_OFFSETOF(Camera::Vftable, fpSetAspectRatio, 0x4C);
    MARATHON_ASSERT_OFFSETOF(Camera::Vftable, fpSetWorldIntersectionCSM, 0x6C);
    MARATHON_ASSERT_OFFSETOF(Camera::Vftable, fpUpdate, 0x8C);
    MARATHON_ASSERT_SIZEOF(Camera::Vftable, 0x90);

    MARATHON_ASSERT_OFFSETOF(Camera, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Camera, 0x04);
}
