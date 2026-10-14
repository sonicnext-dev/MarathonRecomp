#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/CsdObject.h>
#include <Sonicteam/HUDLoadingEndSceneObserver.h>
#include <Sonicteam/HUDLoadingLoopSceneObserver.h>
#include <Sonicteam/HUDLoadingSceneObserver.h>

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
        HUDLoadingSceneObserver m_SceneObserver;
        HUDLoadingLoopSceneObserver m_LoopSceneObserver;
        HUDLoadingEndSceneObserver m_EndSceneObserver;
        be<uint32_t> m_Flags;
    };

    MARATHON_ASSERT_OFFSETOF(HUDLoading, m_spCsdObject, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDLoading, m_SceneObserver, 0x58);
    MARATHON_ASSERT_OFFSETOF(HUDLoading, m_LoopSceneObserver, 0x78);
    MARATHON_ASSERT_OFFSETOF(HUDLoading, m_EndSceneObserver, 0x98);
    MARATHON_ASSERT_OFFSETOF(HUDLoading, m_Flags, 0xB8);
    MARATHON_ASSERT_SIZEOF(HUDLoading, 0xBC);
}
