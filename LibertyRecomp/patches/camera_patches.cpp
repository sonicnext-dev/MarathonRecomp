#include "camera_patches.h"
#include <api/Liberty.h>
#include <patches/aspect_ratio_patches.h>
#include <patches/MainMenuTask_patches.h>
#include <user/config.h>

static class MainMenuCameraUpdateEvent : public ContextHookEvent<Sonicteam::MainMenuTask>
{
public:
    void Update(Sonicteam::MainMenuTask* pThis, float deltaTime) override
    {
        auto pMainMode = pThis->GetDocMode<Sonicteam::MainMode>();

        if (!pMainMode)
            return;

        pMainMode->m_spSelectCamera->Update();
    }
}
g_mainMenuCameraUpdateEvent{};

void CameraPatches::Init()
{
    MainMenuTaskPatches::Events.push_back(&g_mainMenuCameraUpdateEvent);
}

void CameraImp_SetFOV(PPCRegister& f1)
{
    if (g_aspectRatio >= WIDE_ASPECT_RATIO)
        return;

    f1.f64 = 2.0 * atan(tan(0.5 * f1.f64) / g_aspectRatio * WIDE_ASPECT_RATIO);
}

void SonicCamera_InvertAzDriveK(PPCRegister& az_driveK)
{
    // X axis is inverted by default.
    if (Config::HorizontalCamera == ECameraRotationMode::Reverse)
        return;

    az_driveK.f64 = -az_driveK.f64;
}

void SonicCamera_InvertAltDriveK(PPCRegister& alt_driveK)
{
    // Y axis is not inverted by default.
    if (Config::VerticalCamera == ECameraRotationMode::Normal)
        return;

    alt_driveK.f64 = -alt_driveK.f64;
}

void DemoGMCamera_InvertHorizontal(PPCRegister& horz)
{
    // X axis is inverted by default.
    if (Config::HorizontalCamera == ECameraRotationMode::Reverse)
        return;

    horz.f64 = -horz.f64;
}

void DemoGMCamera_InvertVertical(PPCRegister& vert)
{
    // Y axis is inverted by default.
    if (Config::VerticalCamera == ECameraRotationMode::Reverse)
        return;

    vert.f64 = -vert.f64;
}
