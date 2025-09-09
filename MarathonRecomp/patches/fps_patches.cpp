#include <api/Marathon.h>
#include <user/config.h>
#include <app.h>

// Sonicteam::SoX::Physics::Havok::WorldHavok::Update
PPC_FUNC_IMPL(__imp__sub_82587AA8);
PPC_FUNC(sub_82587AA8)
{
    auto pPhysicsWorld = (Sonicteam::SoX::Physics::Havok::WorldHavok*)(base + ctx.r3.u32);

    // Use dynamic update rate if the FPS exceeds the fixed rate.
    pPhysicsWorld->m_IsDynamicUpdateRate = Config::FPS > 120;

    __imp__sub_82587AA8(ctx, base);
}

void PostureControl_RotationSpeedFix(PPCRegister& c_rotation_speed, PPCRegister& stack)
{
    auto deltaTime = *(be<double>*)g_memory.Translate(stack.u32 + 0x200);
    c_rotation_speed.f64 = (c_rotation_speed.f64 * (60.0 * deltaTime));
}

void SonicCamera_RotationSpeedFix(PPCRegister& f0, PPCRegister& deltaTime, PPCRegister& f13)
{
    f0.f64 = float((f0.f64 * (deltaTime.f64 * (1.0 / (deltaTime.f64 * 60.0)))) + f13.f64);
}


bool FixTPJRopeHFR2BNE(PPCRegister& r30)
{
    if (Config::FPS > 60.0)
        return r30.f64 >= 0;
    else
        return false; // original
}

void FixTPJRopeHFR3LFS(PPCRegister& r_value, PPCRegister& r_delta)
{
    if (Config::FPS <= 60.0)
        return;

    r_value.f64 = r_delta.f64;
}
//Not finished
void FixTPJRopeHFR4LFS(PPCRegister& r_value, PPCRegister& r_delta)
{
    if (Config::FPS <= 60.0)
        return;

    r_value.f64 = r_delta.f64;
}
void FixTPJRopeHFR5LFS(PPCRegister& r_value, PPCRegister& r_delta)
{
    if (Config::FPS <= 60.0)
        return;

    r_value.f64 = r_delta.f64;
}
void FixTPJRopeHFR6LFS(PPCRegister& r_value, PPCRegister& r_delta)
{
    if (Config::FPS <= 60.0)
        return;

    r_value.f64 = r_delta.f64;
}

//force to save
void FixTPJRopeHFR7EXTRA(PPCRegister& r_value, PPCRegister& r_value2, PPCRegister& r_stack)
{
    if (Config::FPS <= 60.0)
        return;

    auto deltaTime = *(be<double>*)g_memory.Translate(r_stack.u32 + 0x90 + 0x420 - 0x88);
    r_value.f64 = App::s_deltaTime;
    r_value2.f64 = App::s_deltaTime;
}

//no Tarzan Class implementation, because Fixture in gauge patch o-o
struct TPJ_POINT
{
    MARATHON_INSERT_PADDING(0x4C);
    be<float> m_Time;
    MARATHON_INSERT_PADDING(0xB0);

};

// SonicTeam::TPJ
PPC_FUNC_IMPL(__imp__sub_8232D770);
PPC_FUNC(sub_8232D770)
{
    if (Config::FPS <= 60.0)
    {
        __imp__sub_8232D770(ctx, base);
        return;
    }

    auto pTPJ = (TPJ_POINT*)(base + ctx.r3.u32);
    auto delta = ctx.f1.f64;
    PPCRegister _r4 = ctx.f1;

    if (delta > 1.0)
        delta = 1.0;
    do
    {
        //f1,f2,f3
        GuestToHostFunction<void>(sub_8232D288, pTPJ, _r4.u64,delta,delta,delta);
        pTPJ->m_Time = pTPJ->m_Time - delta;
    } while (pTPJ->m_Time >= delta);
}
