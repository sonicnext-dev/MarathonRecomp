#pragma once

#include <Marathon.inl>
#include <stdx/vector.h>
#include <Sonicteam/Player/ModelStandard.h>
#include <Sonicteam/Player/IDynamicLink.h>

namespace Sonicteam::Player
{
    class Item;

    class ModelSonic : public ModelStandard, public IDynamicLink
    {
    public:
        SoX::RefSharedPointer<SoX::Graphics::FrameGP> m_pFrameGPSonic;
        SoX::RefSharedPointer<SoX::Graphics::FrameGP> m_pFrameGPShoes; // actually it is shoes frame
        boost::shared_ptr<PackageModel::Object> m_spPackageShoesModel;
        stdx::vector<void*> m_vFieldDC;
        boost::shared_ptr<Item> m_spItem; // weak_ptr
        ModelStandardESP m_ESPShoes;
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spShoesRootShadowClump; // ?
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spShoesMouthClump; // ?
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spShoesHeadShadowClump;
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spShoesHeadClump;
        SoX::RefSharedPointer<SoX::Graphics::MaterialAnimationCommon> m_pShoesGemsAnimation;
        SoX::RefSharedPointer<SoX::Graphics::MaterialAnimationCommon> m_pShoesGemsAnimationInstance;
        be<uint32_t> m_CurrentGemsIndex;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_pFrameGPSonic, 0xCC);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_pFrameGPShoes, 0xD0);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_spPackageShoesModel, 0xD4);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_vFieldDC, 0xDC);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_spItem, 0xEC);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_ESPShoes, 0xF4);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_spShoesRootShadowClump, 0x130);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_spShoesMouthClump, 0x134);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_spShoesHeadShadowClump, 0x138);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_spShoesHeadClump, 0x13C);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_pShoesGemsAnimation, 0x140);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_pShoesGemsAnimationInstance, 0x144);
    MARATHON_ASSERT_OFFSETOF(ModelSonic, m_CurrentGemsIndex, 0x148);
   
    //MARATHON_ASSERT_SIZEOF(ModelSonic, 0xCC);
}
