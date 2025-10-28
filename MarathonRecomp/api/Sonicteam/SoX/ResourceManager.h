#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <api/stdx/map.h>

namespace Sonicteam::SoX
{
    struct MARATHON_STD_MAP_CONST_CHAR_COMPARE {
        bool operator()(xpointer<const char> lhs, xpointer<const char> rhs) const {
            return std::strcmp(lhs.get(), rhs.get()) < 0;
        }
    };

    struct ResourceManagerMgrTemplate 
    {
        // void (*Unk0)(Sonicteam::SoX::IResourceMgr*);
        be<uint32_t> fpUnk0; //mostly sub_82595818 (almost any IResourceMgr + 0xC, 0x10 just nullsub) no idea for what

        // void (*Unk4)(Sonicteam::SoX::IResourceMgr*);
        be<uint32_t> fpUnk4; //mostly sub_82631500 (almost any IResourceMgr + 0x10, 0x10 just nullsub) no idea for what

        xpointer< Sonicteam::SoX::IResourceMgr> Mgr;

        char Flag01; //01  (enabled??) ref to 82581FD4, 825BDB54  (when template is created, store or no idea wtf is it)

        MARATHON_INSERT_PADDING(3);
        char Flag02; // ref to 825BDB90, fpUnk4(Mgr) then set Flag02 to 0, for what?
    };

    class ResourceManager : public System::Singleton<ResourceManager, 0x82D3B264, System::CreateStatic<ResourceManager, 0x82581F00>>
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };
        xpointer<Vftable> m_pVftable;
        //m_Resources[IResourceMgr-index][Resource-Name] -> IResource*
        stdx::map<be<uint32_t>, stdx::map<stdx::string, xpointer<Sonicteam::SoX::IResource>>> m_mResource;
        MARATHON_INSERT_PADDING(4); // unk
        MARATHON_INSERT_PADDING(0xC); // std::list<SoX::RefSharedPointer<SoX::RefCountObject>)
        stdx::map<be<uint32_t>, ResourceManagerMgrTemplate> m_mManager;
        MARATHON_INSERT_PADDING(4);
        stdx::map<xpointer<const char>, be<uint32_t>, MARATHON_STD_MAP_CONST_CHAR_COMPARE> m_mManagerIndex;
    };
}
