#include <api/Marathon.h>
#include <user/config.h>

// Sonicteam::SoX::Physics::Havok::WorldHavok::Update
PPC_FUNC_IMPL(__imp__sub_82587AA8);
PPC_FUNC(sub_82587AA8)
{
    auto pPhysicsWorld = (Sonicteam::SoX::Physics::Havok::WorldHavok*)(base + ctx.r3.u32);

    // Use dynamic update rate if the FPS exceeds the fixed rate.
    pPhysicsWorld->m_IsDynamicUpdateRate = Config::FPS > 120;

    __imp__sub_82587AA8(ctx, base);
}

void PostureControlRotationSpeedFix(PPCRegister& c_rotation_speed, PPCRegister& stack)
{
    double deltaTime = *(be<double>*)g_memory.Translate(stack.u32 + 0x200);
    c_rotation_speed.f64 = (c_rotation_speed.f64 * (60.0f * deltaTime));
}
