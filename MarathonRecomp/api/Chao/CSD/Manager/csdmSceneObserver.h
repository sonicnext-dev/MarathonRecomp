#pragma once

#include <Chao/CSD/Manager/csdmObserverBase.h>

namespace Chao::CSD
{
    class CScene;

    class CSceneObserver : public CObserverBase<CScene>
    {
    public:
        struct Vftable : public CObserverBase<CScene>::Vftable
        {
            be<uint32_t> fpOnMotionEnd;
            be<uint32_t> fpOnMotionStart;
            MARATHON_INSERT_PADDING(0x10);
        };
    };

    MARATHON_ASSERT_SIZEOF(CSceneObserver, 0x14);
}
