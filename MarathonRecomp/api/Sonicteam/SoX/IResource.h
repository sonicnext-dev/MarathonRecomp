#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <api/stdx/string.h>

namespace Sonicteam::SoX
{
    class IResource : public RefCountObject
    {
    public:
        struct Vftable:RefCountObject::Vftable
        {
            be<uint32_t> fpInitialize;
            be<uint32_t> fpGetPath; //returns fixed name (player/sonic_new.lua) -> game:\\<xenon|win32|common>/player/sonic_new.lua
            be<uint32_t> fpInArc; 
        };

        void Initialize(void* File,uint64_t Size)
        {
            Vftable* vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpInitialize,this, File,Size);
        }

        stdx::string GetPath(stdx::string* filename)
        {
            Vftable* vft = (Vftable*)m_pVftable.get();
            auto return_value = guest_stack_var<stdx::string>();
            GuestToHostFunction<void>(vft->fpGetPath, return_value.get(), this, filename);
            return *return_value;
        }

        bool InArc()
        {
            Vftable* vft = (Vftable*)m_pVftable.get();
            return GuestToHostFunction<bool>(vft->fpInArc, this);
        }

        be<uint32_t> m_MgrRegistryIndex; //used to deallocate resource
        stdx::string m_MgrResourceName; //m_FilePath
        stdx::string m_ResourceNameInitial;
        stdx::string m_ResourceName;
        bool m_InResourceManager;
        MARATHON_INSERT_PADDING(3);
    };

    MARATHON_ASSERT_OFFSETOF(IResource, m_FilePath, 0x0C);
    MARATHON_ASSERT_SIZEOF(IResource, 0x64);
}
