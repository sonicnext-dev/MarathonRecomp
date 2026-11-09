#pragma once

#include <Marathon.inl>
#include <hk330/hkArray.h>
#include <hk330/hkReferencedObject.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace hk330
{
    class hkBroadPhase;
    class hkBroadPhaseBorderListener;
    class hkCollisionDispatcher;
    class hkCollisionFilter;
    class hkEntityEntityBroadPhaseListener;
    class hkPhantom;
    class hkPhantomBroadPhaseListener;
    class hkProcessCollisionInput;
    class hkRigidBody;
    class hkSimulation;
    class hkSimulationIsland;
    class hkTypedBroadPhaseDispatcher;
    class hkWorldMaintenanceMgr;
    class hkWorldOperationQueue;
    class hkWorldMemoryAvailableWatchDog;

    class hkWorld : public hkReferencedObject
    {
    public:
        xpointer<hkSimulation> m_simulation;
        MARATHON_INSERT_PADDING(0x14);
        Sonicteam::SoX::Math::Vector m_gravity;
        xpointer<hkSimulationIsland> m_fixedIsland;
        xpointer<hkRigidBody> m_fixedRigidBody;
        hkArray<xpointer<hkSimulationIsland>> m_activeSimulationIslands;
        hkArray<xpointer<hkSimulationIsland>> m_inactiveSimulationIslands;
        hkArray<xpointer<hkSimulationIsland>> m_dirtySimulationIslands;
        xpointer<hkWorldMaintenanceMgr> m_maintenanceMgr;
        xpointer<hkWorldMemoryAvailableWatchDog> m_memoryWatchDog;
        xpointer<hkBroadPhase> m_broadPhase;
        xpointer<hkTypedBroadPhaseDispatcher> m_broadPhaseDispatcher;
        xpointer<hkPhantomBroadPhaseListener> m_phantomBroadPhaseListener;
        xpointer<hkEntityEntityBroadPhaseListener> m_entityEntityBroadPhaseListener;
        xpointer<hkBroadPhaseBorderListener> m_broadPhaseBorderListener;
        xpointer<hkProcessCollisionInput> m_collisionInput;
        xpointer<hkCollisionFilter> m_collisionFilter;
        xpointer<hkCollisionDispatcher> m_collisionDispatcher;
        xpointer<hkWorldOperationQueue> m_pendingOperations;
        be<int32_t> m_pendingOperationsCount;
        be<int32_t> m_criticalOperationsLockCount;
        be<int32_t> m_criticalOperationsLockCountForPhantoms;
        bool m_blockExecutingPendingOperations;
        bool m_criticalOperationsAllowed;
        MARATHON_INSERT_PADDING(0x2C);
        hkArray<xpointer<hkPhantom>> m_phantoms;

        void updateCollisionFilterOnWorld(uint32_t updateMode, uint32_t updateShapeCollectionFilter)
        {
            GuestToHostFunction<void>(sub_82832910, this, updateMode, updateShapeCollectionFilter);
        }
    };

    MARATHON_ASSERT_OFFSETOF(hkWorld, m_simulation, 0x08);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_gravity, 0x20);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_fixedIsland, 0x30);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_fixedRigidBody, 0x34);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_activeSimulationIslands, 0x38);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_inactiveSimulationIslands, 0x44);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_dirtySimulationIslands, 0x50);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_maintenanceMgr, 0x5C);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_memoryWatchDog, 0x60);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_broadPhase, 0x64);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_broadPhaseDispatcher, 0x68);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_phantomBroadPhaseListener, 0x6C);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_entityEntityBroadPhaseListener, 0x70);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_broadPhaseBorderListener, 0x74);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_collisionInput, 0x78);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_collisionFilter, 0x7C);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_collisionDispatcher, 0x80);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_pendingOperations, 0x84);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_pendingOperationsCount, 0x88);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_criticalOperationsLockCount, 0x8C);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_criticalOperationsLockCountForPhantoms, 0x90);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_blockExecutingPendingOperations, 0x94);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_criticalOperationsAllowed, 0x95);
    MARATHON_ASSERT_OFFSETOF(hkWorld, m_phantoms, 0xC4);
    MARATHON_ASSERT_SIZEOF(hkWorld, 0xD0);
}
