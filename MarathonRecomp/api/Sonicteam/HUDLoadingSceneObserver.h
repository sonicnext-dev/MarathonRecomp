#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Manager/csdmSceneObserver.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam
{
    class HUDLoading;

    class HUDLoadingSceneObserver : public SoX::RefCountObject, public Chao::CSD::CSceneObserver
    {
    public:
        SoX::RefSharedPointer<HUDLoading> m_spHUDLoading;
    };

    MARATHON_ASSERT_OFFSETOF(HUDLoadingSceneObserver, m_spHUDLoading, 0x1C);
    MARATHON_ASSERT_SIZEOF(HUDLoadingSceneObserver, 0x20);
}
