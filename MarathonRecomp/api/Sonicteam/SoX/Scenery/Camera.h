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
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> fpSetAspectRatio;
            MARATHON_INSERT_PADDING(0x3C);
            be<uint32_t> fpUpdate;
        };

        xpointer<Vftable> m_pVftable;

        int SetFar(double zFar)
        {
            return GuestToHostFunction<int>(m_pVftable->fpSetFar.get(), this, zFar);
        }

        int SetFOV(double fov)
        {
            return GuestToHostFunction<int>(m_pVftable->fpSetFOV.get(), this, fov);
        }

        int SetAspectRatio(float width, float height)
        {
            struct AspectRatio
            {
                be<float> Width;
                be<float> Height;
            };

            guest_stack_var<AspectRatio> aspectRatio(width, height);

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

    MARATHON_ASSERT_OFFSETOF(Camera, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Camera, 4);
}
