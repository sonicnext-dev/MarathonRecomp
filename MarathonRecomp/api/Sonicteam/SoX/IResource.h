#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <stdx/string.h>

namespace Sonicteam::SoX
{
    class IResource : public RefCountObject
    {
    public:
        struct Vftable : RefCountObject::Vftable
        {
            be<uint32_t> fpInit;
            be<uint32_t> fpGetPath;
            be<uint32_t> fpIsInARC; 
        };

        be<uint32_t> m_MgrRegistryIndex; // Used to free resource.
        stdx::string m_MgrResourceName;
        stdx::string m_ResourceNameInitial;
        stdx::string m_ResourceName;
        bool m_IsInResourceManager;
        MARATHON_INSERT_PADDING(3);

        void Init(void* file, uint64_t size)
        {
            GuestToHostFunction<void>(((Vftable*)m_pVftable.get())->fpInit, this, file, size);
        }

        stdx::string GetPath(stdx::string* fileName)
        {
            guest_stack_var<stdx::string> out{};

            GuestToHostFunction<void>(((Vftable*)m_pVftable.get())->fpGetPath, out.get(), this, fileName);

            return *out;
        }

        bool IsInARC()
        {
            return GuestToHostFunction<bool>(((Vftable*)m_pVftable.get())->fpIsInARC, this);
        }
    };

    MARATHON_ASSERT_OFFSETOF(IResource, m_FilePath, 0x0C);
    MARATHON_ASSERT_SIZEOF(IResource, 0x64);
}
