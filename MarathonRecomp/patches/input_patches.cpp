#include <user/config.h>

void RemapLightDash(PPCRegister& r3, PPCRegister& r11)
{
    r11.u64 = ((r3.u32 >> (Config::LightDash == ELightDash::X ? 8 : 15)) & 1) != 0;
}
