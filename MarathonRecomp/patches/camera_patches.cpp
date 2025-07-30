#include <api/Marathon.h>
#include <user/config.h>

// Load Sonicteam::Camera::SonicCamera parameters
PPC_FUNC_IMPL(__imp__sub_82192218);
PPC_FUNC(sub_82192218)
{
    auto pSonicCamera = (Sonicteam::Camera::SonicCamera*)g_memory.Translate(ctx.r3.u32);

    __imp__sub_82192218(ctx, base);

    // X axis is inverted by default.
    if (Config::HorizontalCamera == ECameraRotationMode::Normal)
        pSonicCamera->m_AzDriveK = -pSonicCamera->m_AzDriveK;

    if (Config::VerticalCamera == ECameraRotationMode::Reverse)
        pSonicCamera->m_AltDriveK = -pSonicCamera->m_AltDriveK;
}
