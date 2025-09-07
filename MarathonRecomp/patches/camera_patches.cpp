#include <user/config.h>

void SonicCamera_InvertAzDriveK(PPCRegister& az_driveK)
{
    // X axis is inverted by default.
    if (Config::HorizontalCamera != ECameraRotationMode::Normal)
        return;

    az_driveK.f64 = -az_driveK.f64;
}

void SonicCamera_InvertAltDriveK(PPCRegister& alt_driveK)
{
    if (Config::VerticalCamera != ECameraRotationMode::Reverse)
        return;

    alt_driveK.f64 = -alt_driveK.f64;
}
