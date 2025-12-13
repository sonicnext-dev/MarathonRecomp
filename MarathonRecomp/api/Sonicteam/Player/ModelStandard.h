#pragma once

#include <Marathon.inl>
#include <stdx/vector.h>
#include <Sonicteam/Player/IModel.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/RootFrame.h>
#include <Sonicteam/SoX/Graphics/FrameGP.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/Thread.h>
#include <Sonicteam/Player/IImportAnimation.h>
#include <boost/smart_ptr/shared_ptr.h> 
//#include <Sonicteam/Player/Load.h>
//#include <Sonicteam/SoX/Graphics/Node.h>
#include <Sonicteam/SoX/Graphics/MaterialAnimationCommon.h>
#include <Sonicteam/Player/PackageModel/Object.h>



namespace Sonicteam::SoX::Scenery
{
    class Clump;
}

namespace Sonicteam::Player
{
    class Node;
    class MaterialAnimationInstance;
    class Load;

    enum ESPPartState
    {
        ESPPartState_OnStart = 0,
        ESPPartState_OnLoop = 1,
        ESPPartState_OnEnd = 2,
        ESPPartState_Stop = 3,
    };

    struct ModelStandardESPPart
    {
        be<ESPPartState> ESPState;
        xpointer<Node> pESPNode;
        xpointer<SoX::Graphics::MaterialAnimationCommon> pAnimationESPStart; //sonic_esp_s_Root/Head.xnv
        xpointer<SoX::Graphics::MaterialAnimationCommon> pAnimationESPLoop; //sonic_esp_l_Root/Head.xnv
        xpointer<SoX::Graphics::MaterialAnimationCommon> pAnimationESPEnd; //sonic_esp_e_Root/Head.xnv
        xpointer<MaterialAnimationInstance> pESPInstance;
        bool IsESPDisabled; // just block, if you turn on this and try set aura off it wont go off
    };

    MARATHON_ASSERT_OFFSETOF(ModelStandardESPPart, ESPState, 0);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESPPart, pESPNode, 4);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESPPart, pAnimationESPStart, 8);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESPPart, pAnimationESPLoop, 0xC);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESPPart, pAnimationESPEnd, 0x10);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESPPart, pESPInstance, 0x14);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESPPart, IsESPDisabled, 0x18);


    struct ModelStandardESP
    {
        ModelStandardESPPart ESPRoot;
        ModelStandardESPPart ESPHead;
        bool IsAuraEnabled;
        bool DoAuraUpdate;

        void EnableAura()
        {
            IsAuraEnabled = 1;
            DoAuraUpdate = 1;
        }

        void DisableAura()
        {
            IsAuraEnabled = 0;
            DoAuraUpdate = 1;
        }
    };
    MARATHON_ASSERT_OFFSETOF(ModelStandardESP, ESPRoot, 0);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESP, ESPHead, 0x1C);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESP, IsAuraEnabled, 0x38);
    MARATHON_ASSERT_OFFSETOF(ModelStandardESP, DoAuraUpdate, 0x39);

    struct AnimIDMap 
    {
        be<uint32_t> ID;
        be<uint32_t> Param1;
        be<uint32_t> Extra;
        be<float> FieldC;
        be<float> Field10;
        be<size_t> TargetID; // SPlayerAnimationTable::Animation (Animation)
        be<float> Field18;
    };
    struct UACSATIndexTable
    {
        char buffer[9]; // no idea
    };

    struct SPlayerAnimationTable 
    {
        SoX::ArrayPtr<xpointer<char>> Animation;
        SoX::AIMArray<AnimIDMap> AnimationMAP;
        SoX::ArrayPtr<xpointer<const char>> AnimationHead;
        SoX::AIMArray<AnimIDMap> AnimationHeadMAP;
        SoX::ArrayPtr<UACSATIndexTable> IndexTable;
    };

    class ModelStandard : public IModel, public IVariable, public IStepable, public IFlagCommunicator, public IImportAnimation
    {
    public:
        boost::shared_ptr<PackageModel::Object> m_spPackageModel;
        ModelStandardESP m_ModelESP;
        Sonicteam::SoX::RefSharedPointer<SoX::Scenery::Clump> m_spRootShadowClump;
        Sonicteam::SoX::RefSharedPointer<SoX::Scenery::Clump> m_spMouthClump;
        Sonicteam::SoX::RefSharedPointer<SoX::Scenery::Clump> m_spHeadShadowClump;
        Sonicteam::SoX::RefSharedPointer<SoX::Scenery::Clump> m_spHeadClump;
        be<uint32_t> m_AnimationIDPre;
        be<uint32_t> m_AnimationIDPost;
        be<uint32_t> m_Field8C;
        Sonicteam::SoX::RefSharedPointer<RootFrame> m_spRootFrame;
        Sonicteam::SoX::RefSharedPointer<SoX::Graphics::FrameGP> m_spFrameGPStandard;
        xpointer<Load> m_pPlayerLoad;
        xpointer<SPlayerAnimationTable> m_pAnimationTable;
        stdx::vector<SoX::RefSharedPointer<SoX::RefCountObject>> m_vspFieldA0;
        MARATHON_INSERT_PADDING(0x10);
        xpointer<SoX::Thread> m_pStepableThread; // Model Update Thread
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_spPackageModel, 0x30);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_ModelESP, 0x38);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_spRootShadowClump, 0x74);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_spMouthClump, 0x78);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_spHeadShadowClump, 0x7C);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_spHeadClump, 0x80);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_AnimationIDPre, 0x84);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_AnimationIDPost, 0x88);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_Field8C, 0x8C);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_spRootFrame, 0x90);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_spFrameGPStandard, 0x94);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_pPlayerLoad, 0x98);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_pAnimationTable, 0x9C);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_vspFieldA0, 0xA0);
    MARATHON_ASSERT_OFFSETOF(ModelStandard, m_pStepableThread, 0xC0);
    MARATHON_ASSERT_SIZEOF(ModelStandard, 0xC8);
}
