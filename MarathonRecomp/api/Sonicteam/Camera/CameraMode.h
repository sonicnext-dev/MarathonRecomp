#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Math/Transform.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/Game.h>

namespace Sonicteam::Camera
{
    class CameraMode : public SoX::MessageReceiver
    {
    public:
        struct Vftable : public SoX::MessageReceiver::Vftable
        {
            be<uint32_t> fpUpdate;
            be<uint32_t> fpGetTransform;
            be<uint32_t> fpGetFOV;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpReset;
            MARATHON_INSERT_PADDING(0x08);
        };

        enum CameraType
        {
            CameraType_CameraTargetAtPosition,
            CameraType_LineChaseCamera,
            CameraType_FixCamera,
            CameraType_FixChaseCamera,
            CameraType_ReverseCamera
        };

        boost::shared_ptr<Game> m_spGame;
        xpointer<void> m_pInputListener;
        MARATHON_INSERT_PADDING(0x10);
        be<CameraType> m_Type;
        xpointer<SoX::Engine::Doc> m_pDoc;

        SoX::Math::Transform GetTransform()
        {
            const auto result = guest_stack_var<SoX::Math::Transform>();

            GuestToHostFunction<void*>(static_cast<Vftable*>(m_pVftable.get())->fpGetTransform.get(), result.get(), this);

            return *result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(CameraMode::Vftable, fpUpdate, 0x08);
    MARATHON_ASSERT_OFFSETOF(CameraMode::Vftable, fpGetTransform, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CameraMode::Vftable, fpGetFOV, 0x10);
    MARATHON_ASSERT_OFFSETOF(CameraMode::Vftable, fpReset, 0x18);
    MARATHON_ASSERT_SIZEOF(CameraMode::Vftable, 0x24);

    MARATHON_ASSERT_OFFSETOF(CameraMode, m_spGame, 0x04);
    MARATHON_ASSERT_OFFSETOF(CameraMode, m_pInputListener, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CameraMode, m_Type, 0x20);
    MARATHON_ASSERT_OFFSETOF(CameraMode, m_pDoc, 0x24);
    MARATHON_ASSERT_SIZEOF(CameraMode, 0x28);
}
