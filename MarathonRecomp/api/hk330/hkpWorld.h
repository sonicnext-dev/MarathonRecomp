#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <hk330/hkArray.h>
#include <hk330/hkMultiThreadCheck.h>

namespace hk330
{
    class hkpRigidBody;
    class hkpPhantom;
    class hkpRigidBody;
    class hkpCollisionFilter;
    class hkpWorld : public hkReferencedObject
    {
    public:
        xpointer<void> m_simulation;
        Sonicteam::SoX::Math::Vector m_gravity;
        xpointer<void> m_fixedIsland;
        xpointer<hkpRigidBody> m_fixedRigidBody;
        hkArray<void> m_activeSimulationIslands;
        hkArray<void> m_inactiveSimulationIslands;
        hkArray<void> m_dirtySimulationIslands;
        xpointer<void> m_maintenanceMgr;
        xpointer<void> m_memoryWatchDog;
        MARATHON_INSERT_PADDING(8);
        xpointer<void> m_broadPhase; //should be at this right position
        xpointer<void> m_broadPhaseDispatcher;
        xpointer<void> m_phantomBroadPhaseListener;
        xpointer<void> m_entityEntityBroadPhaseListener;
        xpointer<void> m_broadPhaseBorderListener;
        //xpointer<void> m_multithreadedSimulationJobData;
        //Some of them are are not need (duno why) 4 bytes
        xpointer<void> m_collisionInput;
        xpointer<hkpCollisionFilter> m_collisionFilter;
      //  xpointer<void> m_collisionDispatcher; probably (not need)
        xpointer<void> m_convexListFilter;
        MARATHON_INSERT_PADDING(8);
        xpointer<void> m_pendingOperations; // Correct Place
        be<int32_t> m_pendingOperationsCount; //Correct Place
        be<int32_t> m_criticalOperationsLockCount;
        be<int32_t> m_criticalOperationsLockCountForPhantoms;
        be<bool> m_blockExecutingPendingOperations; //Correct Place 
        be<bool> m_criticalOperationsAllowed;
        xpointer<void> hkpDebugInfoOnPendingOperationQueues;
        be<int32_t> m_pendingOperationQueueCount;
        hkMultiThreadCheck m_multiThreadCheck;
        be<bool> m_processActionsInSingleThread;
        be<uint32_t> m_minDesiredIslandSize;
        xpointer<void> m_modifyConstraintCriticalSection;
        xpointer<void> m_worldLock;
        xpointer<void> m_islandDirtyListCriticalSection;
        xpointer<void> m_propertyMasterLock;
        be<bool> m_wantSimulationIslands;
        be<float> m_snapCollisionToConvexEdgeThreshold;
        be<float> m_snapCollisionToConcaveEdgeThreshold;
        be<float> m_enableToiWeldRejection;
        be<float> m_wantDeactivation;
        be<float> m_shouldActivateOnRigidBodyTransformChange;
        be<float> m_highFrequencyDeactivationPeriod;
        be<float> m_lowFrequencyDeactivationPeriod;
        be<float> m_deactivationReferenceDistance;
        be<float> m_toiCollisionResponseRotateNormal;
        be<int32_t> m_simulationType;
        be<uint32_t> m_lastEntityUid;
        hkArray<xpointer<hkpPhantom>> hkpPhantom;
  

        //sub_828306E0
        //hkWorld_updateFilterOnSinglePhantom 8282D528 
        //hkpworld::internal_executePendingOperations 8282DB94 
        void updateCollisionFilterOnPhantom(hk330::hkpPhantom* phantom, uint32_t updateShapeCollectionFilter);
    };
}
