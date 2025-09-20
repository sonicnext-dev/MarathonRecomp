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
        MARATHON_INSERT_PADDING(20);
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
}
