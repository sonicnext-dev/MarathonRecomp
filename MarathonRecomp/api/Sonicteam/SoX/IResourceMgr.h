#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/ResourceManager.h>
#include <stdx/string.h>

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
            be<uint32_t> fpCreate;
            be<uint32_t> fpGetPath;
            MARATHON_INSERT_PADDING(0x08);
        };

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_ID;
        bool m_IsInitialized;
        bool m_Field09;
        be<uint32_t> m_Field0C;

        void* Destroy(const uint32_t in_flags = 1)
        {
            return GuestToHostFunction<void*>(m_pVftable->fpDestroy, this, in_flags);
        }

        IResource* Create(const IResourceMgrCreationParams& in_rParams)
        {
            return GuestToHostFunction<IResource*>(m_pVftable->fpCreate, this, &in_rParams);
        }

        stdx::string GetPath(const char* in_pFileName)
        {
            const auto result = guest_stack_var<stdx::string>();
            const auto fileName = guest_stack_var<stdx::string>(in_pFileName);

            GuestToHostFunction<void>(m_pVftable->fpGetPath, result.get(), this, fileName.get());

            return *result;
        }

        auto& GetResources() const
        {
            return ResourceManager::GetInstance()->m_Resources[m_ID];
        }

        template <typename T = IResource>
        T* GetResource(const char* in_pName)
        {
            if (!m_IsInitialized)
                return nullptr;

            auto& rResources = GetResources();
            const auto it = rResources.find(in_pName);

            if (it == rResources.end())
                return nullptr;

            return static_cast<T*>(it->second.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(IResourceMgr, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(IResourceMgr, m_ID, 0x04);
    MARATHON_ASSERT_OFFSETOF(IResourceMgr, m_IsInitialized, 0x08);
    MARATHON_ASSERT_OFFSETOF(IResourceMgr, m_Field09, 0x09);
    MARATHON_ASSERT_OFFSETOF(IResourceMgr, m_Field0C, 0x0C);
    MARATHON_ASSERT_SIZEOF(IResourceMgr, 0x10);
}
