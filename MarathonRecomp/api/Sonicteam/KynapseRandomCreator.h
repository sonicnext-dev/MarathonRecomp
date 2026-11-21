#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <Kaim/CEntityEyePosition.h>
#include <Kaim/CEntityVisualAcuteness.h>
#include <Kaim/CSpatialGraph.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/CKynapseCommonTraversal.h>
#include <Sonicteam/Game.h>
#include <Sonicteam/KyCameraEntity.h>
#include <Sonicteam/KynapseUtility.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class CTownsKeeper;

    class KynapseRandomCreator
    {
    public:
        struct AppearanceRatio
        {
            stdx::string Race;
            be<uint32_t> Ratio;
        };

        xpointer<void> m_pVftable;
        boost::weak_ptr<Game> m_wpGame;
        xpointer<CTownsKeeper> m_pTownsKeeper;
        xpointer<SoX::Physics::World> m_pWorld;
        stdx::vector<be<uint32_t>> m_TownsmanActorIDs;
        stdx::vector<be<uint32_t>> m_Field24;
        MARATHON_INSERT_PADDING(0x10); // stdx::vector<T>
        MARATHON_INSERT_PADDING(0x10); // stdx::vector<T>
        be<uint32_t> m_Field54;
        stdx::vector<be<uint32_t>> m_Field58;
        stdx::vector<be<float>> m_Field68;
        be<uint32_t> m_Field78;
        MARATHON_INSERT_PADDING(0x10); // stdx::vector<T>
        be<uint32_t> m_TownsmanCount;
        be<uint32_t> m_VisibleTownsmanCount;
        be<uint32_t> m_Random;
        be<uint32_t> m_GeneratePeopleMax;
        be<uint32_t> m_FrameInMax;
        be<uint32_t> m_FieldA0;
        be<uint32_t> m_ManTypeMax;
        xpointer<AppearanceRatio> m_pAppearanceRatios;
        bool m_GenerateControl;
        bool m_FrameControl;
        xpointer<CKynapseCommonTraversal> m_pKynapseCommonTraversal;
        boost::shared_ptr<KynapseUtility> m_spKynapseUtility;
        xpointer<KyCameraEntity> m_pKyCameraEntity;
        xpointer<Kaim::CEntityVisualAcuteness> m_pVisualAcuteness;
        xpointer<Kaim::CEntityEyePosition> m_pEyePosition;
        xpointer<Kaim::CSpatialGraph> m_pSpatialGraph;
    };

    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_wpGame, 0x04);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pTownsKeeper, 0x0C);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pWorld, 0x10);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_TownsmanActorIDs, 0x14);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_Field24, 0x24);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_Field54, 0x54);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_Field58, 0x58);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_Field68, 0x68);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_Field78, 0x78);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_TownsmanCount, 0x8C);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_VisibleTownsmanCount, 0x90);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_Random, 0x94);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_GeneratePeopleMax, 0x98);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_FrameInMax, 0x9C);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_FieldA0, 0xA0);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_ManTypeMax, 0xA4);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pAppearanceRatios, 0xA8);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_GenerateControl, 0xAC);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_FrameControl, 0xAD);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pKynapseCommonTraversal, 0xB0);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_spKynapseUtility, 0xB4);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pKyCameraEntity, 0xBC);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pVisualAcuteness, 0xC0);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pEyePosition, 0xC4);
    MARATHON_ASSERT_OFFSETOF(KynapseRandomCreator, m_pSpatialGraph, 0xC8);
    MARATHON_ASSERT_SIZEOF(KynapseRandomCreator, 0xCC);
}
