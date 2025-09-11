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
    if (Config::FPS <= 60.0f)
        return false;

    return r30.f64 >= 0;
}

void ObjTarzan_PatchStaticDeltaTime(PPCRegister& value, PPCRegister& delta)
{
    if (Config::FPS <= 60.0f)
        return;

    value.f64 = delta.f64;
}

void ObjTarzan_PatchDeltaTimeArgument(PPCRegister& value, PPCRegister& value2, PPCRegister& stack)
{
    if (Config::FPS <= 60.0f)
        return;

    auto deltaTime = *(be<double>*)g_memory.Translate(stack.u32 + 0x90 + 0x420 - 0x88);

    value.f64 = deltaTime;
    value2.f64 = deltaTime;
}

// Sonicteam::ObjTarzan::UpdatePoint (speculatory)
PPC_FUNC_IMPL(__imp__sub_8232D770);
PPC_FUNC(sub_8232D770)
{
    if (Config::FPS <= 60.0f)
    {
        __imp__sub_8232D770(ctx, base);
        return;
    }

    struct TarzanPoint
    {
        MARATHON_INSERT_PADDING(0x4C);
        be<float> m_Time;
        MARATHON_INSERT_PADDING(0xB0);
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

struct ObjectMessage_SuckPlayerToPointEX :public Sonicteam::Player::ObjectMessage_SuckPlayerToPoint
{
public:
    be<float> m_delta;
};

void ObjectInputWarp_MessageToPlayerExtraDelta(PPCRegister& Phantom, PPCRegister& Message, PPCRegister& delta)
{
    auto pPhantom = (Sonicteam::SoX::Physics::Phantom*)(g_memory.Translate(Phantom.u32));
    auto pMessage = (Sonicteam::Player::ObjectMessage_SuckPlayerToPoint*)(g_memory.Translate(Message.u32));
    auto pNewMessage = (ObjectMessage_SuckPlayerToPointEX*)g_userHeap.Alloc(sizeof(ObjectMessage_SuckPlayerToPointEX));
    pNewMessage->m_ID = pMessage->m_ID;
    pNewMessage->m_Point = pMessage->m_Point;
    pNewMessage->m_delta = delta.f64;
    pPhantom->OnMessageRecieved(pNewMessage); 
    g_userHeap.Free(pNewMessage);
}

void ObjectPlayer_Message0x1104AExtraDelta(PPCRegister& delta, PPCRegister& Message)
{
    auto pMessage = (ObjectMessage_SuckPlayerToPointEX*)(g_memory.Translate(Message.u32));
    delta.f64 = pMessage->m_delta;
}
