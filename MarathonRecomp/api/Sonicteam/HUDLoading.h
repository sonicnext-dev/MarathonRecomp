#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/CsdObject.h>

namespace Sonicteam
{
    class HUDLoading : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        enum HUDLoadingFlags
        {
            HUDLoadingFlags_Finished = 6,
            HUDLoadingFlags_Open = 0x200,
            HUDLoadingFlags_End = 0x400
        };

        SoX::RefSharedPointer<CsdObject> m_spCsdObject;
        MARATHON_INSERT_PADDING(0x58);
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_OFFSETOF(HUDLoading, m_spCsdObject, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDLoading, m_Flags, 0xB0);
    MARATHON_ASSERT_SIZEOF(HUDLoading, 0xBC);
}
