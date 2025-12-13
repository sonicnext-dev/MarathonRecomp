#pragma once

#include <Liberty.inl>
#include <hk330/hkArray.h>
#include <hk330/hkReferencedObject.h>

namespace hk330
{
    // Simple Vector type for Havok physics (replaces Sonicteam::SoX::Math::Vector)
    struct hkVector4
    {
        be<float> x, y, z, w;
    };

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
        LIBERTY_INSERT_PADDING(0x14);
        hkVector4 m_gravity;
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
        LIBERTY_INSERT_PADDING(0x2C);
        hkArray<xpointer<hkpPhantom>> m_phantoms;

        void updateCollisionFilterOnWorld(uint32_t updateMode, uint32_t updateShapeCollectionFilter)
        {
            // TODO: Find GTA IV equivalent function address
            // GuestToHostFunction<void>(sub_XXXXXXXX, this, updateMode, updateShapeCollectionFilter);
        }
    };

    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_simulation, 0x08);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_gravity, 0x20);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_fixedIsland, 0x30);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_fixedRigidBody, 0x34);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_activeSimulationIslands, 0x38);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_inactiveSimulationIslands, 0x44);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_dirtySimulationIslands, 0x50);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_maintenanceMgr, 0x5C);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_memoryWatchDog, 0x60);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_broadPhase, 0x64);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_broadPhaseDispatcher, 0x68);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_phantomBroadPhaseListener, 0x6C);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_entityEntityBroadPhaseListener, 0x70);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_broadPhaseBorderListener, 0x74);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_collisionInput, 0x78);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_collisionFilter, 0x7C);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_collisionDispatcher, 0x80);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_pendingOperations, 0x84);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_pendingOperationsCount, 0x88);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_criticalOperationsLockCount, 0x8C);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_criticalOperationsLockCountForPhantoms, 0x90);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_blockExecutingPendingOperations, 0x94);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_criticalOperationsAllowed, 0x95);
    LIBERTY_ASSERT_OFFSETOF(hkpWorld, m_phantoms, 0xC4);
}
