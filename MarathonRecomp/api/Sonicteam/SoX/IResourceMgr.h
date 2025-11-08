#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>

namespace Sonicteam::SoX
{
    // Different for each implementation.
    struct IResourceMgrCreationParams;

    class IResourceMgr
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpCreateResource;
            be<uint32_t> fpGetPath;
            MARATHON_INSERT_PADDING(8);
        };

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_MgrIndex;
        MARATHON_INSERT_PADDING(8);

        void* Destroy(uint32_t flag)
        {
            return GuestToHostFunction<void*>(m_pVftable->fpDestroy, this, flag);
        }

        Sonicteam::SoX::IResource* CreateResource(IResourceMgrCreationParams& param)
        {
            return GuestToHostFunction<Sonicteam::SoX::IResource*>(m_pVftable->fpCreateResource, this, &param);
        }

        stdx::string GetPath(stdx::string* fileName)
        {
            guest_stack_var<stdx::string> out{};

            GuestToHostFunction<void>(m_pVftable->fpGetPath, out.get(), this, fileName);

            return *out;
        }
    };

    MARATHON_ASSERT_OFFSETOF(IResourceMgr, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IResourceMgr, 0x10);
}
