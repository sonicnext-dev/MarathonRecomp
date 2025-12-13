#include <api/Liberty.h>
#include <user/config.h>
#include <app.h>

constexpr double REFERENCE_DELTA_TIME = 1.0 / 60.0;

// Only use this in threaded context with fixed delta time!
double MakeDeltaTime(std::chrono::steady_clock::time_point& prev)
{
    auto now = std::chrono::steady_clock::now();
    auto deltaTime = std::min(std::chrono::duration<double>(now - prev).count(), 1.0 / 15.0);

    prev = now;
    now = std::chrono::steady_clock::now();

    return deltaTime;
}

bool HasFrameElapsed(double reference, double& timeElapsed, double deltaTime)
{
    timeElapsed += deltaTime;

    if (timeElapsed < reference)
        return false;

    timeElapsed = 0.0f;

    return true;
}

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

bool ObjTarzan_VolatileBranch(PPCRegister& r30)
{
    if (Config::FPS <= 60)
        return false;

    return r30.f64 >= 0;
}

void ObjTarzan_PatchStaticDeltaTime(PPCRegister& value, PPCRegister& delta)
{
    if (Config::FPS <= 60)
        return;

    value.f64 = delta.f64;
}

void ObjTarzan_PatchDeltaTimeArgument(PPCRegister& value, PPCRegister& value2, PPCRegister& stack)
{
    if (Config::FPS <= 60)
        return;

    auto deltaTime = *(be<double>*)g_memory.Translate(stack.u32 + 0x90 + 0x420 - 0x88);

    value.f64 = deltaTime;
    value2.f64 = deltaTime;
}

// Sonicteam::ObjTarzan::UpdatePoint (speculatory)
PPC_FUNC_IMPL(__imp__sub_8232D770);
PPC_FUNC(sub_8232D770)
{
    if (Config::FPS <= 60)
    {
        __imp__sub_8232D770(ctx, base);
        return;
    }

    struct TarzanPoint
    {
        LIBERTY_INSERT_PADDING(0x4C);
        be<float> m_Time;
        LIBERTY_INSERT_PADDING(0xB0);
    };

    auto pTarzanPoint = (TarzanPoint*)(base + ctx.r3.u32);
    auto deltaTime = ctx.f1.f64;

    if (deltaTime > 1.0)
        deltaTime = 1.0;

    do
    {
        GuestToHostFunction<void>(sub_8232D288, pTarzanPoint, ctx.f1.u64, deltaTime, deltaTime, deltaTime);
        pTarzanPoint->m_Time = pTarzanPoint->m_Time - deltaTime;
    }
    while (pTarzanPoint->m_Time >= deltaTime);
}

void ObjEspSwing_DecayRateFix(PPCRegister& f0, PPCRegister& f13, PPCRegister& deltaTime)
{
    f0.f64 = float(f13.f64 * pow(pow(f0.f64, 60.0), deltaTime.f64));
}

struct MsgSuckPlayerEx : public Sonicteam::Message::MsgSuckPlayer
{
    be<float> DeltaTime;
};

void ObjectInputWarp_ExtendMsgSuckPlayer(PPCRegister& phantom, PPCRegister& message, PPCRegister& deltaTime)
{
    auto pPhantom = (Sonicteam::SoX::Physics::Phantom*)g_memory.Translate(phantom.u32);
    auto pMessage = (Sonicteam::Message::MsgSuckPlayer*)g_memory.Translate(message.u32);

    auto pMsgSuckPlayerEx = (MsgSuckPlayerEx*)g_userHeap.Alloc(sizeof(MsgSuckPlayerEx));
    pMsgSuckPlayerEx->ID = pMessage->ID;
    pMsgSuckPlayerEx->Point = pMessage->Point;
    pMsgSuckPlayerEx->DeltaTime = deltaTime.f64;

    pPhantom->ProcessMessage(pMsgSuckPlayerEx);

    g_userHeap.Free(pMsgSuckPlayerEx);
}

void PlayerObject_ProcessMsgSuckPlayer_FixForce(PPCRegister& message, PPCRegister& force)
{
    auto pMessage = (MsgSuckPlayerEx*)g_memory.Translate(message.u32);

    force.f64 = pow(force.f64, pMessage->DeltaTime * 60.0);
}

void PlayerObject_ProcessMsgSuckPlayer_FixDeltaTime(PPCRegister& message, PPCRegister& deltaTime)
{
    auto pMessage = (MsgSuckPlayerEx*)g_memory.Translate(message.u32);

    deltaTime.f64 = pMessage->DeltaTime;
}

void Spanverse_GE1PE_DeltaTimeFix(PPCRegister& deltaTime)
{
    deltaTime.f64 = App::s_deltaTime;
}

// Allocate more space to store the previous loading
// time for this instance of Sonicteam::HUDLoading.
void HUDLoadingAlloc(PPCRegister& r3)
{
    r3.u32 += sizeof(std::chrono::steady_clock::time_point);
}

// Sonicteam::HUDLoading::HUDLoading
PPC_FUNC_IMPL(__imp__sub_824D7BC8);
PPC_FUNC(sub_824D7BC8)
{
    auto pPrevLoadingTime = (std::chrono::steady_clock::time_point*)(base + ctx.r3.u32 + sizeof(Sonicteam::HUDLoading));

    *pPrevLoadingTime = {};

    __imp__sub_824D7BC8(ctx, base);
}

// Accumulate own delta time and provide it to the CSD update function.
void HUDLoading_DeltaTimeFix(PPCRegister& pThis, PPCRegister& deltaTime)
{
    auto pPrevLoadingTime = (std::chrono::steady_clock::time_point*)g_memory.Translate(pThis.u32 + sizeof(Sonicteam::HUDLoading));

    deltaTime.f64 = MakeDeltaTime(*pPrevLoadingTime);
}

// This function is an override of Sonicteam::SoX::Engine::Task::Update,
// it does not pass delta time to HUDCALLBACK::Update, so we must preserve
// the register here in order to do it ourselves.
void HUDWindow_PreserveDeltaTime(PPCRegister& f31, PPCRegister& f1)
{
    f31.f64 = f1.f64;
}

// HUDCALLBACK::Update doesn't have a delta time argument,
// so we pass one in here to do some delta time accumulation
// safely in the hooks below.
void HUDWindow_Callback(PPCRegister& f1, PPCRegister& f31)
{
    f1.f64 = f31.f64;
}

// Sonicteam::PriceListWindowTask::HUDCALLBACK::Update
PPC_FUNC_IMPL(__imp__sub_8250AAB0);
PPC_FUNC(sub_8250AAB0)
{
    static auto s_time = 0.0;

    // Fix for white ⇄ red text breathing animation.
    if (!HasFrameElapsed(REFERENCE_DELTA_TIME, s_time, ctx.f1.f64))
        return;

    __imp__sub_8250AAB0(ctx, base);
}

// Sonicteam::SelectWindowTask::HUDCALLBACK::Update
PPC_FUNC_IMPL(__imp__sub_8250D698);
PPC_FUNC(sub_8250D698)
{
    static auto s_time = 0.0;

    // Fix for white ⇄ red text breathing animation.
    if (!HasFrameElapsed(REFERENCE_DELTA_TIME, s_time, ctx.f1.f64))
        return;

    __imp__sub_8250D698(ctx, base);
}

bool ObjectVehicleBike_EnemyShot_DisableVehicleCollisionLayer(PPCRegister& r3)
{
    if (Config::FPS <= 60)
        return false;

    auto pObjectVehicleBike = (Sonicteam::ObjectVehicleBike*)g_memory.Translate(r3.u32);
    auto pPrefixDependency = pObjectVehicleBike->m_pDependency->GetFirstDependency();

    // Call original function.
    sub_822C1FA8(*GetPPCContext(), g_memory.base);

    auto pPostfixDependency = pObjectVehicleBike->m_pDependency->GetFirstDependency();
    auto isUpdatePhysicsWorld = false;

    // Process rigid bodies created after the function call.
    while (pPrefixDependency != pPostfixDependency)
    {
        pPrefixDependency = pPrefixDependency->m_pPrevSibling.get();

        if (strcmp(pPrefixDependency->GetName(), "EnemyShotNormal") == 0)
        {
            auto pEnemyShotNormal = (Sonicteam::Enemy::EnemyShotNormal*)pPrefixDependency;

            if (auto pPhantom = pEnemyShotNormal->m_spPhantom.get())
            {
                // Fix bullets colliding with the bike at high frame rates.
                auto& collidable = pPhantom->m_pRigidBody->m_collidable;
                collidable.m_broadPhaseHandle.m_collisionFilterInfo = 0xFFFF5E00;
                isUpdatePhysicsWorld = true;
            }
        }
    }

    if (isUpdatePhysicsWorld)
    {
        if (auto pGameMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>())
        {
            auto pWorldHavok = pGameMode->GetGame()->GetPhysicsWorld<Sonicteam::SoX::Physics::Havok::WorldHavok>();

            if (auto pWorld = pWorldHavok->m_pWorld)
                pWorld->updateCollisionFilterOnWorld(1, 1);
        }
    }

    return true;
}
