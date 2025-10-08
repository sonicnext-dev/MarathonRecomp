#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Scenery/Clump.h>

namespace Sonicteam::Player
{
    class ILoad : public IPlugIn
    {
    public:
        struct Vftable : IPlugIn::Vftable
        {
            be<uint32_t> fpLoadPackage;
            be<uint32_t> fpConnectListenerToInput;
            be<uint32_t> fpSyncFrame;
            be<uint32_t> fpProcessClump01;
            be<uint32_t> fpAddClump;
            be<uint32_t> fpProcessClump02_01;
            be<uint32_t> fpAddShadowClump;
            be<uint32_t> fpILoadUnknown;
            be<uint32_t> fpProcessClump02_03;
            be<uint32_t> fpGetGraphicDevice;
            be<uint32_t> fpGetParticleManager;
            be<uint32_t> fpSendMessageToSFXAgent;
            be<uint32_t> fpAddThread;
        };

        SoX::RefSharedPointer<Package> LoadPackage(stdx::string* package)
        {
            guest_heap_var<SoX::RefSharedPointer<Package>> r = reinterpret_cast<Package*>(0);
            MARATHON_CALL_VIRTUAL_FUNCTION(IPlugIn, void, &Vftable::fpLoadPackage, r.get(), this, package);
            return *r;
        }

        SoX::RefSharedPointer<Package> LoadPackage(stdx::string& package)
        {
            guest_heap_var<SoX::RefSharedPointer<Package>> r = reinterpret_cast<Package*>(0);
            guest_heap_var<stdx::string> s(package);
            MARATHON_CALL_VIRTUAL_FUNCTION(IPlugIn, void, &Vftable::fpLoadPackage, r.get(), this, s.get());
            return *r;
        }

        void AddFrame(SoX::Graphics::Frame* pClump)
        {
            auto spclump = guest_heap_var<SoX::RefSharedPointer<SoX::Graphics::Frame>>(pClump);
            MARATHON_CALL_VIRTUAL_FUNCTION(IPlugIn, void, &Vftable::fpSyncFrame, this,spclump.get());
        }

        void AddClump(SoX::Scenery::Clump* pClump)
        {
            auto spclump = guest_heap_var<SoX::RefSharedPointer<SoX::Scenery::Clump>>(pClump);
            MARATHON_CALL_VIRTUAL_FUNCTION(IPlugIn, void, &Vftable::fpAddClump, this,spclump.get());
        }

        void AddShadowClump(SoX::Scenery::Clump* pClump)
        {
            auto spclump = guest_heap_var<SoX::RefSharedPointer<SoX::Scenery::Clump>>(pClump);
            MARATHON_CALL_VIRTUAL_FUNCTION(IPlugIn, void, &Vftable::fpAddShadowClump, this,spclump.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpLoadPackage, 4);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpConnectListenerToInput, 8);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpSyncFrame, 0xC);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpProcessClump01, 0x10);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpAddClump, 0x14);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpProcessClump02_01, 0x18);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpAddShadowClump, 0x1C);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpILoadUnknown, 0x20);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpProcessClump02_03, 0x24);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpGetGraphicDevice, 0x28);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpGetParticleManager, 0x2C);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpSendMessageToSFXAgent, 0x30);
    MARATHON_ASSERT_OFFSETOF(ILoad::Vftable, fpAddThread, 0x34);
    MARATHON_ASSERT_SIZEOF(ILoad, 0x20);
  
}
