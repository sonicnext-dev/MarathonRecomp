#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>

namespace Sonicteam::SoX
{

    //different for each implementation
    struct IResourceMgrCreationParam
    {

    };

    class IResourceMgr
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpCreateResource;
            be<uint32_t> fpGetPath;
            MARATHON_INSERT_PADDING(0x8);
        };

        xpointer<Vftable> m_pVftable;

        void* Destroy(uint32_t flag)
        {
            return GuestToHostFunction<void*>(m_pVftable->fpDestroy, this, flag);
        }

        Sonicteam::SoX::IResource* CreateResource(IResourceMgrCreationParam& param)
        {
            return GuestToHostFunction<Sonicteam::SoX::IResource*>(m_pVftable->fpCreateResource, this, &param);
        }

        stdx::string GetPath(stdx::string* filename)
        {
            auto return_value = guest_stack_var<stdx::string>();
            GuestToHostFunction<void>(m_pVftable->fpGetPath, return_value.get(), this, filename);;
            return *return_value;
        }

        be<uint32_t> m_MgrIndex;
        MARATHON_INSERT_PADDING(8);
    };
}
