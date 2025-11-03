#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <api/stdx/map.h>

namespace Sonicteam::SoX
{
    struct ResourceManagerMgrTemplate
    {
        be<uint32_t> Field00; // Usually a function pointer to 0x82595818.
        be<uint32_t> Field04; // Usually a function pointer to 0x82631500.
        xpointer<Sonicteam::SoX::IResourceMgr> pManager;
        char Flag01; // 01 (enabled?), referenced in 82581FD4 and 825BDB54 (when template is created).
        MARATHON_INSERT_PADDING(3);
        char Flag02; // Referenced in 825BDB90 and *Field04, then set Flag02 to 0, for what?
    };

    class ResourceManager : public System::Singleton<ResourceManager, 0x82D3B264, System::CreateStatic<ResourceManager, 0x82581F00>>
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        stdx::map<be<uint32_t>, stdx::map<stdx::string, xpointer<Sonicteam::SoX::IResource>>> m_mResources; // m_mResources[IResourceMgr->m_MgrIndex][Resource->m_Name] -> IResource*
        MARATHON_INSERT_PADDING(4);
        MARATHON_INSERT_PADDING(0x0C); // std::list<SoX::RefSharedPointer<SoX::RefCountObject>>
        stdx::map<be<uint32_t>, ResourceManagerMgrTemplate> m_mManagers;
        MARATHON_INSERT_PADDING(4);
        stdx::map<xpointer<const char>, be<uint32_t>, MARATHON_STD_MAP_CONST_CHAR_COMPARE> m_mManagerIndices;
    };
}
