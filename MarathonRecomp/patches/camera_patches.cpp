#include "camera_patches.h"
#include <api/Marathon.h>
#include <hid/peripheral/gamepad.h>
#include <hid/peripheral/keyboard.h>
#include <hid/peripheral/mouse.h>
#include <hid/hid.h>
#include <patches/aspect_ratio_patches.h>
#include <patches/MainMenuTask_patches.h>
#include <user/config.h>
#include <app.h>

static class MainMenuCameraUpdateEvent : public ContextHookEvent<Sonicteam::MainMenuTask>
{
public:
    void Update(Sonicteam::MainMenuTask* pThis, float deltaTime) override
    {
        auto pMainMode = pThis->GetParent<Sonicteam::MainMode>();

        if (!pMainMode)
            return;

        pMainMode->m_spSelectCamera->Update();
    }
}
g_mainMenuCameraUpdateEvent{};

void CameraPatches::Init()
{
    MainMenuTaskPatches::s_events.push_back(&g_mainMenuCameraUpdateEvent);
}

void CameraImp_SetFOV(PPCRegister& f1)
{
    if (g_aspectRatio >= WIDE_ASPECT_RATIO)
        return;

    f1.f64 = 2.0 * atan(tan(0.5 * f1.f64) / g_aspectRatio * WIDE_ASPECT_RATIO);
}

void SonicCamera_InvertAzDrive(PPCRegister& f12)
{
    // X is inverted by default.
    if (Config::HorizontalCamera == ECameraRotationMode::Reverse)
        return;
    
    f12.f64 = -f12.f64;
}

void SonicCamera_InvertAltDrive(PPCRegister& f12)
{
    // Y is not inverted by default.
    if (Config::VerticalCamera == ECameraRotationMode::Normal)
        return;
    
    f12.f64 = -f12.f64;
}

void DemoGMCamera_InvertYaw(PPCRegister& f0)
{
    // X is inverted by default.
    if (Config::HorizontalCamera == ECameraRotationMode::Reverse)
        return;

    f0.f64 = -f0.f64;
}

void DemoGMCamera_InvertPitch(PPCRegister& f0)
{
    // Y is inverted by default.
    if (Config::VerticalCamera == ECameraRotationMode::Reverse)
        return;

    f0.f64 = -f0.f64;
}

// Tuned for 50% sensitivity to be a solid middle ground for the default.
static constexpr float k_mouseSensitivityBase = 0.00345f;

static bool GetRelativeMouseForPlayer(const uint32_t in_playerIndex, float* out_x, float* out_y)
{
    bool result{};
    float x{}, y{};

    const auto pUser = HID::GetManager()->GetUser(in_playerIndex);

    if (pUser)
    {
        if (const auto pMouse = static_cast<HID::Mouse*>(pUser->Find(HID::Mouse::ID)))
        {
            // Scale sensitivity exponentially to make increments a little less egregious in the upper half.
            const auto sensitivity = std::lerp(0.01f, 1.0f, std::powf(Config::MouseSensitivity, 3.0f));

            x = pMouse->RelativeX * sensitivity;
            y = pMouse->RelativeY * sensitivity;

            if (Config::HorizontalCamera == ECameraRotationMode::Reverse)
                x = -x;

            if (Config::VerticalCamera == ECameraRotationMode::Reverse)
                y = -y;

            result = true;
        }
    }

    if (out_x)
        *out_x = x;

    if (out_y)
        *out_y = y;

    return result;
}

class SonicCameraEx : public Sonicteam::Camera::SonicCamera
{
public:
    enum MouseState : uint8_t
    {
        MouseState_Disabled,
        MouseState_Resetting,
        MouseState_Enabled
    };

    enum MouseOrbitAxis : uint8_t
    {
        MouseOrbitAxis_Distance = (0 << 0),
        MouseOrbitAxis_Yaw = (1 << 0),
        MouseOrbitAxis_Pitch = (1 << 1)
    };

    MouseState m_MouseState;
    Sonicteam::SoX::Math::Vector m_MouseTransitionPosition;
    float m_MouseTransitionTime;
    float m_MouseInactiveTime;
    Sonicteam::SoX::Math::Vector m_MouseOrbit;

    void ConfigureMouse()
    {
        m_MouseState = SonicCameraEx::MouseState_Disabled;
        m_MouseTransitionPosition = m_Position;
        m_MouseTransitionTime = 0.0f;
        m_MouseInactiveTime = Config::MouseAutoCenterTime;
        m_MouseOrbit = m_Orbit;
    }

    void ResetMouseOrbit(std::underlying_type_t<MouseOrbitAxis> in_resetAxes)
    {
        if (in_resetAxes)
        {
            const auto dx = m_Position.X - m_Target.X;
            const auto dy = m_Position.Y - m_Target.Y;
            const auto dz = m_Position.Z - m_Target.Z;

            if ((in_resetAxes & MouseOrbitAxis_Yaw) != 0)
                m_MouseOrbit.Y = std::atan2(dx, dz);

            if ((in_resetAxes & MouseOrbitAxis_Pitch) != 0)
            {
                m_MouseOrbit.Z = Sonicteam::SoX::Math::ToRadians(-10.0f);
            }
            else
            {
                m_MouseOrbit.Z = -std::atan2(dy, std::sqrt(dx * dx + dz * dz));
            }
        }

        StartMouseTransition(m_Position);
    }

    void SyncMouseToOrbit()
    {
        m_MouseOrbit.Y = m_Orbit.Y;
        m_MouseOrbit.Z = m_Orbit.Z;
    }

    void SyncOrbitToMouse()
    {
        m_Orbit.Y = m_MouseOrbit.Y;
        m_Orbit.Z = m_MouseOrbit.Z;
    }

    void StartMouseTransition(const Sonicteam::SoX::Math::Vector& in_rStart)
    {
        m_MouseTransitionPosition = in_rStart - m_Target;
        m_MouseTransitionTime = 0.0f;
    }

    bool IsPlayerOutOfControl()
    {
        const auto pGame = static_cast<Sonicteam::GameImp*>(m_spGame.get());

        const auto playerIndex = pGame->GetPlayerIndexFromActorID(m_TargetActorID);
        const auto pPlayer = pGame->GetPlayer(playerIndex);

        if (!pPlayer)
            return false;

        const auto pStateMachine = pPlayer->GetStateMachine<Sonicteam::Player::State::Machine2>();

        if (!pStateMachine)
            return false;

        const auto pStateMachineContext = pStateMachine->GetContext<Sonicteam::Player::State::CommonContext>();
        
        if (!pStateMachineContext)
            return false;

        const auto flags = pStateMachineContext->m_CommonContextIFFlags3.get();

        const auto miscFlags = pStateMachineContext->GetMiscFlags(Sonicteam::Player::State::CommonMiscFlagsSlot_100) &&
                               pStateMachineContext->GetMiscFlags(Sonicteam::Player::State::CommonMiscFlagsSlot_10C);

        const auto isCommonBalancer = pStateMachine->GetState()->m_pVftable.ptr == 0x820038F0;

        return (flags & Sonicteam::Player::CommonContextIFFlags_OutOfControl) != 0 || miscFlags && !isCommonBalancer;
    }
};

void SonicCamera_Alloc(PPCRegister& r3)
{
    r3.u32 = sizeof(SonicCameraEx);
}

// Sonicteam::Camera::SonicCamera::LoadParams
PPC_FUNC_IMPL(__imp__sub_82192218);
PPC_FUNC(sub_82192218)
{
    const auto pSonicCamera = reinterpret_cast<SonicCameraEx*>(base + ctx.r3.u32);

    __imp__sub_82192218(ctx, base);

    pSonicCamera->ConfigureMouse();
}

void SonicCamera_MouseLook(PPCRegister& r31, PPCRegister& r5, PPCRegister& in_deltaTime)
{
    if (App::s_isPaused)
        return;

    const auto pSonicCamera = reinterpret_cast<SonicCameraEx*>(g_memory.Translate(r31.u32));

    // Only enable mouse look for the standard camera.
    if (pSonicCamera->m_Type != Sonicteam::Camera::CameraMode::CameraType_CameraTargetAtPosition)
        return;

    const auto pOutput = reinterpret_cast<Sonicteam::SoX::Math::Vector*>(g_memory.Translate(r5.u32));

    const auto pGame = static_cast<Sonicteam::GameImp*>(pSonicCamera->m_spGame.get());
    const auto playerIndex = pGame->GetPlayerIndexFromActorID(pSonicCamera->m_TargetActorID);

    float x{}, y{};
    const auto hasMouse = GetRelativeMouseForPlayer(playerIndex, &x, &y);

    // Yaw and pitch are inverted for this camera.
    x = -x;
    y = -y;

    if (!hasMouse)
        return;

    switch (pSonicCamera->m_MouseState)
    {
        case SonicCameraEx::MouseState_Disabled:
        {
            if (std::fabsf(x) > 0.0f || std::fabsf(y) > 0.0f)
            {
                // Sync current camera orbit to mouse orbit.
                pSonicCamera->SyncMouseToOrbit();
                pSonicCamera->StartMouseTransition(*pOutput);
                pSonicCamera->m_MouseState = SonicCameraEx::MouseState_Enabled;
            }

            return;
        }

        // Sync current mouse orbit to camera orbit.
        case SonicCameraEx::MouseState_Resetting:
            pSonicCamera->SyncOrbitToMouse();
            pSonicCamera->ResetMouseOrbit(SonicCameraEx::MouseOrbitAxis_Yaw);
            pSonicCamera->m_MouseState = SonicCameraEx::MouseState_Disabled;
            return;

        case SonicCameraEx::MouseState_Enabled:
        {
            const auto pMutator = HID::GetManager()->GetUser(playerIndex)->GetMutator();

            if (pMutator && pMutator->GetType() == HID::PeripheralType::Gamepad)
            {
                const auto padState = pMutator->GetState();

                // Right stick moved beyond threshold, reset back to original camera behaviour.
                if (std::abs(padState.sThumbRX) >= 8000 || std::abs(padState.sThumbRY) >= 8000)
                {
                    pSonicCamera->m_MouseState = SonicCameraEx::MouseState_Resetting;
                    return;
                }
            }

            if (Config::MouseAutoCenter)
            {
                if (std::fabsf(x) > 0.0f || std::fabsf(y) > 0.0f)
                {
                    pSonicCamera->m_MouseInactiveTime = 0.0f;
                }
                else
                {
                    pSonicCamera->m_MouseInactiveTime += App::s_deltaTime;
                }

                if (pSonicCamera->m_MouseInactiveTime >= Config::MouseAutoCenterTime)
                {
                    pSonicCamera->m_MouseState = SonicCameraEx::MouseState_Resetting;
                    return;
                }
            }
            else
            {
                pSonicCamera->m_MouseInactiveTime = 0.0f;
            }

            if (pSonicCamera->IsPlayerOutOfControl())
            {
                // Fall back to acceleration-based mouse look.
                pSonicCamera->m_Acceleration.X = x;
                pSonicCamera->m_Acceleration.Y = y;

                pSonicCamera->ResetMouseOrbit(SonicCameraEx::MouseOrbitAxis_Yaw);
                pSonicCamera->StartMouseTransition(*pOutput);

                return;
            }

            break;
        }
    }

    static constexpr float maxPitch = Sonicteam::SoX::Math::ToRadians(45.0f);

    // Maps relative mouse X/Y to expected values for the orbit vector.
    // X = Distance, Y = Yaw (wrapped pi squared), Z = Pitch (45 degrees clamped).
    // This vector is usually used for interpolating the camera position to look ahead of the player.
    pSonicCamera->m_MouseOrbit.Y = Sonicteam::SoX::Math::WrapAngle(pSonicCamera->m_MouseOrbit.Y + (x * k_mouseSensitivityBase));
    pSonicCamera->m_MouseOrbit.Z = std::clamp(pSonicCamera->m_MouseOrbit.Z + (y * k_mouseSensitivityBase), -maxPitch, maxPitch);
    pSonicCamera->m_Orbit = pSonicCamera->m_MouseOrbit;

    auto normalisedOrbit = guest_stack_var<Sonicteam::SoX::Math::Vector>();

    GuestToHostFunction<Sonicteam::SoX::Math::Vector*>(sub_8218C4F0, normalisedOrbit.get(), &pSonicCamera->m_Orbit);

    // Defines the shape cast distance to be from the player to the orbit position at max distance.
    auto distance = guest_stack_var<Sonicteam::SoX::Math::Distance>(pSonicCamera->m_Target, pSonicCamera->m_Target + *normalisedOrbit);

    auto context = guest_stack_var<Sonicteam::Camera::SonicCamera::MoveShapeCastListener::MoveShapeCastContext>();
    auto listener = guest_stack_var<Sonicteam::Camera::SonicCamera::MoveShapeCastListener>(context.get());

    // Scales the distance of the camera to the player based on the shape cast hit position.
    if (pSonicCamera->m_pShapeCastA->CastShape(distance.get(), listener.get()))
        pSonicCamera->m_Orbit.X = std::min(context->HitPosition.Distance(pSonicCamera->m_Target), pSonicCamera->m_MouseOrbit.X.get());

    GuestToHostFunction<Sonicteam::SoX::Math::Vector*>(sub_8218C4F0, normalisedOrbit.get(), &pSonicCamera->m_Orbit);

    const auto offset = Sonicteam::SoX::Math::Hermite(pSonicCamera->m_MouseTransitionPosition, *normalisedOrbit, pSonicCamera->m_MouseTransitionTime, 2.75f, in_deltaTime.f64);

    *pOutput = pSonicCamera->m_Target + offset;
}

// Sonicteam::Camera::SonicCamera::Reset
PPC_FUNC_IMPL(__imp__sub_82191200);
PPC_FUNC(sub_82191200)
{
    const auto pSonicCamera = reinterpret_cast<SonicCameraEx*>(base + ctx.r3.u32);

    __imp__sub_82191200(ctx, base);

    pSonicCamera->ResetMouseOrbit(SonicCameraEx::MouseOrbitAxis_Yaw | SonicCameraEx::MouseOrbitAxis_Pitch);
    pSonicCamera->SyncOrbitToMouse();
}

// Sonicteam::Camera::SonicCamera::EndInterpolate
PPC_FUNC_IMPL(__imp__sub_82190710);
PPC_FUNC(sub_82190710)
{
    const auto pSonicCamera = reinterpret_cast<SonicCameraEx*>(base + ctx.r3.u32);

    __imp__sub_82190710(ctx, base);

    pSonicCamera->ResetMouseOrbit(SonicCameraEx::MouseOrbitAxis_Yaw);
    pSonicCamera->SyncOrbitToMouse();
}

// Sonicteam::Camera::DemoGMCamera::Update
PPC_FUNC_IMPL(__imp__sub_8218CAC0);
PPC_FUNC(sub_8218CAC0)
{
    if (App::s_isPaused)
    {
        __imp__sub_8218CAC0(ctx, base);
        return;
    }

    const auto pDemoGMCamera = reinterpret_cast<Sonicteam::Camera::DemoGMCamera*>(base + ctx.r3.u32);
    const auto pGame = static_cast<Sonicteam::GameImp*>(pDemoGMCamera->m_spGame.get());

    float x{}, y{};
    GetRelativeMouseForPlayer(pGame->GetPlayerIndexFromActorID(pDemoGMCamera->m_TargetActorID), &x, &y);

    pDemoGMCamera->m_Yaw = pDemoGMCamera->m_Yaw + (x * k_mouseSensitivityBase);
    pDemoGMCamera->m_Pitch = pDemoGMCamera->m_Pitch + (y * k_mouseSensitivityBase);

    __imp__sub_8218CAC0(ctx, base);
}

// Sonicteam::Camera::SnipeCamera::Update
PPC_FUNC_IMPL(__imp__sub_8218EEB8);
PPC_FUNC(sub_8218EEB8)
{
    if (App::s_isPaused)
    {
        __imp__sub_8218EEB8(ctx, base);
        return;
    }

    const auto pSnipeCamera = reinterpret_cast<Sonicteam::Camera::SnipeCamera*>(base + ctx.r3.u32);

    // do you think god stays in heaven,
    // because he, too, lives in fear of what he's created?
    const auto pPlayer = pSnipeCamera->m_rPlayer.m_Node.m_pData->m_pRef;

    float x{}, y{};
    GetRelativeMouseForPlayer(pPlayer->GetPlayerIndex(), &x, &y);

    pSnipeCamera->m_Yaw = pSnipeCamera->m_Yaw + (-x * k_mouseSensitivityBase);
    pSnipeCamera->m_Pitch = pSnipeCamera->m_Pitch + (y * k_mouseSensitivityBase);

    __imp__sub_8218EEB8(ctx, base);
}
