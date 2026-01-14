#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Graphics/InstanceModelCommon.h>
#include <Sonicteam/SoX/Scenery/Clump.h>
#include <Sonicteam/SoX/Graphics/AnimationCore/SubAnimation.h>
#include <stdx/vector.h>
#include <stdx/string.h>


namespace Sonicteam::Player
{
    struct ItemPoint
    {
        be<uint32_t> m_Count; // ?
        stdx::string m_AttachPoint; // Left_ItemPoint
        MARATHON_INSERT_PADDING(4);
        SoX::RefSharedPointer<SoX::Graphics::InstanceModelCommon> m_spInstanceModel; //player/sonic_new/so_itm_bbungle_L.xno
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spClump;    //player/sonic_new/so_itm_bbungle_L.xno
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spClump2;   //player/sonic_new/so_itm_bbungle_L.xno
    };

    MARATHON_ASSERT_SIZEOF(ItemPoint, 0x30);

    class Load;
    class RootFrame;

    class Item : public IPlugIn, public IDynamicLink, public IStepable, public IFlagCommunicator, public IVariable
    {
    public:
        xpointer<Load> m_pLoad;
        SoX::RefSharedPointer<RootFrame> m_spParentFrame;
        stdx::vector<ItemPoint> m_pvItemPoints;
        be<uint32_t> m_Flag; // 00 1F FF FF (not attached) 80 1F FF FF (attached)
    };

    MARATHON_ASSERT_OFFSETOF(Item, m_pLoad,0x30);
    MARATHON_ASSERT_OFFSETOF(Item, m_spParentFrame,0x34);
    MARATHON_ASSERT_OFFSETOF(Item, m_pvItemPoints,0x38);
    MARATHON_ASSERT_OFFSETOF(Item, m_Flag,0x48);
    MARATHON_ASSERT_SIZEOF(Item, 0x4C);
}
