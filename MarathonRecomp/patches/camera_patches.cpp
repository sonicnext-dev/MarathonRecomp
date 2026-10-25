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
    enum MouseLookState : uint8_t
    {
        MouseLookState_Disabled,
        MouseLookState_Resetting,
        MouseLookState_Enabled
    };

    MouseLookState m_MouseLookState;
    Sonicteam::SoX::Math::Vector m_InterpolateOffset;
    float m_InterpolateTime;
    float m_InactiveTime;
    float m_Distance;
    float m_Yaw;
    float m_Pitch;

    void ResetYawPitch(bool in_useDefaultPitch = false)
    {
        const auto dx = m_Position.X - m_Target.X;
        const auto dy = m_Position.Y - m_Target.Y;
        const auto dz = m_Position.Z - m_Target.Z;

        m_Yaw = std::atan2(dx, dz);

        if (in_useDefaultPitch)
        {
            m_Pitch = -10.0f * DEG2RADf;
        }
        else
        {
            m_Pitch = -std::atan2(dy, std::sqrt(dx * dx + dz * dz));
        }
    }

    void SetInterpolateStart(const Sonicteam::SoX::Math::Vector& in_rStart)
    {
        m_InterpolateOffset = in_rStart - m_Target;
        m_InterpolateTime = 0.0f;
    }

    bool IsPlayerOutOfControl()
    {
        const auto pGame = static_cast<Sonicteam::GameImp*>(m_spGame.get());

        const auto playerIndex = pGame->GetPlayerIndexFromActorID(m_TargetActorID);
        const auto pPlayer = pGame->GetPlayer(playerIndex);

        if (!pPlayer)
            return false;

        const auto pStateMachine = pPlayer->GetStateMachine<Sonicteam::Player::State::Machine2>();
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

    pSonicCamera->m_Distance = pSonicCamera->m_Orbit.X;
}

void SonicCamera_MouseLook(PPCRegister& r31, PPCRegister& r5, PPCRegister& in_deltaTime)
{
    if (App::s_isPaused)
        return;

    const auto pSonicCamera = reinterpret_cast<SonicCameraEx*>(g_memory.Translate(r31.u32));
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

    switch (pSonicCamera->m_MouseLookState)
    {
        case SonicCameraEx::MouseLookState_Disabled:
        {
            if (std::fabsf(x) >= 0.0f && std::fabsf(y) >= 0.0f)
            {
                // Sync current camera orbit to mouse look.
                pSonicCamera->m_Yaw = pSonicCamera->m_Orbit.Y;
                pSonicCamera->m_Pitch = pSonicCamera->m_Orbit.Z;

                pSonicCamera->SetInterpolateStart(*pOutput);

                pSonicCamera->m_MouseLookState = SonicCameraEx::MouseLookState_Enabled;

                break;
            }
            else
            {
                return;
            }
        }

        case SonicCameraEx::MouseLookState_Resetting:
        {
            // Sync current mouse look to camera orbit.
            pSonicCamera->m_Orbit.X = pSonicCamera->m_Distance;
            pSonicCamera->m_Orbit.Y = pSonicCamera->m_Yaw;
            pSonicCamera->m_Orbit.Z = pSonicCamera->m_Pitch;

            pSonicCamera->ResetYawPitch();

            pSonicCamera->m_MouseLookState = SonicCameraEx::MouseLookState_Disabled;

            return;
        }

        case SonicCameraEx::MouseLookState_Enabled:
        {
            const auto pMutator = HID::GetManager()->GetUser(playerIndex)->GetMutator();

            if (pMutator && pMutator->GetType() == HID::PeripheralType::Gamepad)
            {
                const auto padState = pMutator->GetState();

                // Right stick moved beyond threshold, reset back to original camera behaviour.
                if (std::abs(padState.sThumbRX) >= 8000 && std::abs(padState.sThumbRY) >= 8000)
                {
                    pSonicCamera->m_MouseLookState = SonicCameraEx::MouseLookState_Resetting;
                    return;
                }
            }

            if (pSonicCamera->IsPlayerOutOfControl())
            {
                // Fall back to acceleration-based mouse look.
                pSonicCamera->m_Acceleration.X = x;
                pSonicCamera->m_Acceleration.Y = y;

                pSonicCamera->ResetYawPitch();
                pSonicCamera->SetInterpolateStart(*pOutput);

                return;
            }

            break;
        }
    }

    if (Config::MouseAutoCenter)
    {
        if (std::fabsf(x) > 0.0f || std::fabsf(y) > 0.0f)
        {
            pSonicCamera->m_InactiveTime = 0.0f;
        }
        else
        {
            pSonicCamera->m_InactiveTime += App::s_deltaTime;
        }

        if (pSonicCamera->m_InactiveTime >= Config::MouseAutoCenterTime)
        {
            pSonicCamera->m_MouseLookState = SonicCameraEx::MouseLookState_Resetting;
            return;
        }
    }
    else
    {
        pSonicCamera->m_InactiveTime = 0.0f;
    }

    static constexpr float maxPitch = 45.0f * DEG2RADf;

    // Maps relative mouse X/Y to expected values for the orbit vector.
    // X = Distance, Y = Yaw (wrapped pi squared), Z = Pitch (45 degrees clamped).
    // This vector is usually used for interpolating the camera position to look ahead of the player.
    pSonicCamera->m_Orbit.X = pSonicCamera->m_Distance;
    pSonicCamera->m_Orbit.Y = pSonicCamera->m_Yaw = Sonicteam::SoX::Math::WrapAngle(pSonicCamera->m_Yaw + (x * k_mouseSensitivityBase));
    pSonicCamera->m_Orbit.Z = pSonicCamera->m_Pitch = std::clamp(pSonicCamera->m_Pitch + (y * k_mouseSensitivityBase), -maxPitch, maxPitch);

    auto normalisedOrbit = guest_stack_var<Sonicteam::SoX::Math::Vector>();

    GuestToHostFunction<Sonicteam::SoX::Math::Vector*>(sub_8218C4F0, normalisedOrbit.get(), &pSonicCamera->m_Orbit);

    // Defines the shape cast distance to be from the player to the orbit position at max distance.
    auto distance = guest_stack_var<Sonicteam::SoX::Math::Distance>(pSonicCamera->m_Target, pSonicCamera->m_Target + *normalisedOrbit);

    auto context = guest_stack_var<Sonicteam::Camera::SonicCamera::MoveShapeCastListener::MoveShapeCastContext>();
    auto listener = guest_stack_var<Sonicteam::Camera::SonicCamera::MoveShapeCastListener>(context.get());

    // Scales the distance of the camera to the player based on the shape cast hit position.
    if (pSonicCamera->m_pShapeCastA->CastShape(distance.get(), listener.get()))
        pSonicCamera->m_Orbit.X = std::min(context->HitPosition.DistanceTo(pSonicCamera->m_Target), pSonicCamera->m_Distance);

    GuestToHostFunction<Sonicteam::SoX::Math::Vector*>(sub_8218C4F0, normalisedOrbit.get(), &pSonicCamera->m_Orbit);

    const auto offset = Sonicteam::SoX::Math::Hermite(pSonicCamera->m_InterpolateOffset, *normalisedOrbit, pSonicCamera->m_InterpolateTime, 2.75f, in_deltaTime.f64);

    *pOutput = pSonicCamera->m_Target + offset;
}

// Sonicteam::Camera::SonicCamera::Reset
PPC_FUNC_IMPL(__imp__sub_82191200);
PPC_FUNC(sub_82191200)
{
    const auto pSonicCamera = reinterpret_cast<SonicCameraEx*>(base + ctx.r3.u32);

    __imp__sub_82191200(ctx, base);

    pSonicCamera->ResetYawPitch(true);
    pSonicCamera->m_Orbit.X = pSonicCamera->m_Distance;
    pSonicCamera->m_Orbit.Y = pSonicCamera->m_Yaw;
    pSonicCamera->m_Orbit.Z = pSonicCamera->m_Pitch;
}

// Sonicteam::Camera::SonicCamera::EndInterpolate
PPC_FUNC_IMPL(__imp__sub_82190710);
PPC_FUNC(sub_82190710)
{
    const auto pSonicCamera = reinterpret_cast<SonicCameraEx*>(base + ctx.r3.u32);

    __imp__sub_82190710(ctx, base);

    // TODO: check this.
    pSonicCamera->ResetYawPitch();
    pSonicCamera->m_Orbit.Y = pSonicCamera->m_Yaw;
    pSonicCamera->m_Orbit.Z = pSonicCamera->m_Pitch;
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
