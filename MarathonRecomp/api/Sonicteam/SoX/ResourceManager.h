#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>
#include <stdx/compare/string_compare.h>
#include <stdx/list.h>
#include <stdx/map.h>

namespace Sonicteam::SoX
{
    class IResourceMgr;

    class ResourceManager : public System::Singleton<ResourceManager, 0x82D3B264, System::CreateStatic<ResourceManager, 0x82581F00>>
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        struct ResourceMgrInfo
        {
            be<uint32_t> fpFunc00; // Usually a function pointer to 0x82595818.
            be<uint32_t> fpFunc04; // Usually a function pointer to 0x82631500.
            xpointer<IResourceMgr> pResourceMgr;
            uint8_t Field0C;       // Referenced by 0x82581FD4 and 0x825BDB54, usually 1. "IsInitialized"?
            MARATHON_INSERT_PADDING(0x03);
            uint8_t Field10;       // Referenced by 0x825BDB90 and *fpFunc04, then set to 0. "IsLoading"?
        };

        xpointer<Vftable> m_pVftable;
        stdx::map<be<uint32_t>, stdx::map<stdx::string, xpointer<IResource>, stdx::string_compare>> m_Resources; // Key = Manager ID, Value = { Key = Name, Value = Resource }
        MARATHON_INSERT_PADDING(0x04);
        stdx::list<SoX::RefSharedPointer<SoX::RefCountObject>> m_Field14;
        stdx::map<be<uint32_t>, ResourceMgrInfo> m_Managers;
        MARATHON_INSERT_PADDING(0x04);
        stdx::map<xpointer<const char>, be<uint32_t>, stdx::string_compare> m_ManagerIDs;
    };

    MARATHON_ASSERT_OFFSETOF(ResourceManager::ResourceMgrInfo, fpFunc00, 0x00);
    MARATHON_ASSERT_OFFSETOF(ResourceManager::ResourceMgrInfo, fpFunc04, 0x04);
    MARATHON_ASSERT_OFFSETOF(ResourceManager::ResourceMgrInfo, pResourceMgr, 0x08);
    MARATHON_ASSERT_OFFSETOF(ResourceManager::ResourceMgrInfo, Field0C, 0x0C);
    MARATHON_ASSERT_OFFSETOF(ResourceManager::ResourceMgrInfo, Field10, 0x10);
    MARATHON_ASSERT_SIZEOF(ResourceManager::ResourceMgrInfo, 0x14);

    MARATHON_ASSERT_OFFSETOF(ResourceManager, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(ResourceManager, m_Resources, 0x04);
    MARATHON_ASSERT_OFFSETOF(ResourceManager, m_Field14, 0x14);
    MARATHON_ASSERT_OFFSETOF(ResourceManager, m_Managers, 0x20);
    MARATHON_ASSERT_OFFSETOF(ResourceManager, m_ManagerIDs, 0x30);
    MARATHON_ASSERT_SIZEOF(ResourceManager, 0x3C);
}
