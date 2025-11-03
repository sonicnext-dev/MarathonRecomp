#pragma once

#include <Marathon.inl>
#include <hk330/hkArray.h>
#include <hk330/hkReferencedObject.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace hk330
{
    class hkpBroadPhase;
    class hkpBroadPhaseBorderListener;
    class hkpCollisionDispatcher;
    class hkpCollisionFilter;
    class hkpEntityEntityBroadPhaseListener;
    class hkpPhantom;
    class hkpPhantomBroadPhaseListener;
    class hkpProcessCollisionInput;
    class hkpRigidBody;
    class hkpSimulation;
    class hkpSimulationIsland;
    class hkpTypedBroadPhaseDispatcher;
    class hkpWorldMaintenanceMgr;
    class hkpWorldOperationQueue;
    class hkWorldMemoryAvailableWatchDog;

    class hkpWorld : public hkReferencedObject
    {
    public:
        xpointer<hkpSimulation> m_simulation;
        MARATHON_INSERT_PADDING(0x14);
        Sonicteam::SoX::Math::Vector m_gravity;
        xpointer<hkpSimulationIsland> m_fixedIsland;
        xpointer<hkpRigidBody> m_fixedRigidBody;
        hkArray<xpointer<hkpSimulationIsland>> m_activeSimulationIslands;
        hkArray<xpointer<hkpSimulationIsland>> m_inactiveSimulationIslands;
        hkArray<xpointer<hkpSimulationIsland>> m_dirtySimulationIslands;
        xpointer<hkpWorldMaintenanceMgr> m_maintenanceMgr;
        xpointer<hkWorldMemoryAvailableWatchDog> m_memoryWatchDog;
        xpointer<hkpBroadPhase> m_broadPhase;
        xpointer<hkpTypedBroadPhaseDispatcher> m_broadPhaseDispatcher;
        xpointer<hkpPhantomBroadPhaseListener> m_phantomBroadPhaseListener;
        xpointer<hkpEntityEntityBroadPhaseListener> m_entityEntityBroadPhaseListener;
        xpointer<hkpBroadPhaseBorderListener> m_broadPhaseBorderListener;
        xpointer<hkpProcessCollisionInput> m_collisionInput;
        xpointer<hkpCollisionFilter> m_collisionFilter;
        xpointer<hkpCollisionDispatcher> m_collisionDispatcher;
        xpointer<hkpWorldOperationQueue> m_pendingOperations;
        be<int32_t> m_pendingOperationsCount;
        be<int32_t> m_criticalOperationsLockCount;
        be<int32_t> m_criticalOperationsLockCountForPhantoms;
        bool m_blockExecutingPendingOperations;
        bool m_criticalOperationsAllowed;
        MARATHON_INSERT_PADDING(0x2C);
        hkArray<xpointer<hkpPhantom>> m_phantoms;

        void updateCollisionFilterOnWorld(uint32_t updateMode, uint32_t updateShapeCollectionFilter)
        {
            GuestToHostFunction<void>(sub_82832910, this, updateMode, updateShapeCollectionFilter);
        }
    };

    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_simulation, 0x08);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_gravity, 0x20);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_fixedIsland, 0x30);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_fixedRigidBody, 0x34);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_activeSimulationIslands, 0x38);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_inactiveSimulationIslands, 0x44);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_dirtySimulationIslands, 0x50);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_maintenanceMgr, 0x5C);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_memoryWatchDog, 0x60);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_broadPhase, 0x64);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_broadPhaseDispatcher, 0x68);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_phantomBroadPhaseListener, 0x6C);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_entityEntityBroadPhaseListener, 0x70);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_broadPhaseBorderListener, 0x74);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_collisionInput, 0x78);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_collisionFilter, 0x7C);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_collisionDispatcher, 0x80);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_pendingOperations, 0x84);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_pendingOperationsCount, 0x88);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_criticalOperationsLockCount, 0x8C);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_criticalOperationsLockCountForPhantoms, 0x90);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_blockExecutingPendingOperations, 0x94);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_criticalOperationsAllowed, 0x95);
    MARATHON_ASSERT_OFFSETOF(hkpWorld, m_phantoms, 0xC4);
}
