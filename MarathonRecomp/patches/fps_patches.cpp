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

void ObjVehicleBike_BulletDisableCollisionLayer(PPCRegister& r3)
{
    if (Config::FPS <= 60.0)
        return;

    auto ctx = GetPPCContext();
    auto base = g_memory.base;
    auto pVehicleBike = (Sonicteam::ObjectVehicleBike*)(g_memory.Translate(r3.u32));
    auto pNamedActorEntity = (Sonicteam::NamedActor*)(pVehicleBike->m_pDependency.get());
    auto pEnemyShotNormal = (Sonicteam::Enemy::EnemyShotNormal*)pNamedActorEntity->GetFirstDependent();
    sub_822C1FA8(*ctx, g_memory.base);
    auto pEnemyShotNormalAfter = (Sonicteam::Enemy::EnemyShotNormal*)pNamedActorEntity->GetFirstDependent();
    while (pEnemyShotNormal != pEnemyShotNormalAfter)
    {
        pEnemyShotNormal = ((Sonicteam::Enemy::EnemyShotNormal*)pEnemyShotNormal->m_pSiblingPrev.get());
        auto pObject = reinterpret_cast<Sonicteam::SoX::Object*>(pEnemyShotNormal);

        if (pObject->m_pVftable.ptr.get() == 0x820200C8)
        {
            auto hkpCollideable = &pEnemyShotNormal->m_spPhantom->m_pRigidBody->m_collidable;
            auto owner = hkpCollideable->m_parent.get();
         //   pEnemyShotNormal->m_spPhantom->m_spWorldHavok->

            printf("hkpCollideable %x\n", hkpCollideable);
            printf("owner %x\n", owner);
            printf("m_collisionFilterInfo : %x\n", pEnemyShotNormal->m_spPhantom->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo.get()); 
            printf("hkPhantom : %x\n", pEnemyShotNormal->m_spPhantom->m_pRigidBody->m_collidable);
            pEnemyShotNormal->m_spPhantom->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo = 0xFFFF0000;
            printf("pObject : %x\n", pObject);
        }

    }
 
    /*
    if (pVehicleBike->m_pVehicleMissileCtrlAutomatic->m_IsShot)
    {
        pEnemyShotNormal->m_spPhantom->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo = 0x5E00;
        printf("SHOT\n");
    }
    */
    printf("pEntityB : %x\n", pNamedActorEntity);
    printf("pEntityF : %x\n", pEnemyShotNormalAfter);

    printf("NamedActor : %s\n", pNamedActorEntity->m_Name.c_str());
    printf("NamedActor : %s\n", pNamedActorEntity->m_Name.c_str());
    printf("pEntity : %x\n", pEnemyShotNormal);
    printf("m_pVehicleMissileCtrlAutomatic : %x\n", pVehicleBike->m_pVehicleMissileCtrlAutomatic->m_pMyPhantom.get());
    printf("m_pShotInfo : %x\n", pVehicleBike->m_pVehicleMissileCtrlAutomatic->m_pShotInfo.get());
    printf("m_pShotParameter : %x\n", pVehicleBike->m_pVehicleMissileCtrlAutomatic->m_pShotParameter.get());
 
}
