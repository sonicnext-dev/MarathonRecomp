#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Manager/csdmSceneObserver.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam
{
    class HUDLoading;

    class HUDLoadingEndSceneObserver : public SoX::RefCountObject, public Chao::CSD::CSceneObserver
    {
    public:
        SoX::RefSharedPointer<HUDLoading> m_spHUDLoading;
    };

    MARATHON_ASSERT_OFFSETOF(HUDLoadingEndSceneObserver, m_spHUDLoading, 0x1C);
    MARATHON_ASSERT_SIZEOF(HUDLoadingEndSceneObserver, 0x20);
}
