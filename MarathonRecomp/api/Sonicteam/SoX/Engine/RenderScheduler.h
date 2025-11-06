#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/RenderProcess.h>
#include <stdx/list.h>
#include <stdx/wstring.h>

namespace Sonicteam::SoX::Engine
{
    class RenderScheduler
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFunc4;
        };

        xpointer<Vftable> m_pVftable;
        stdx::list<std::pair<stdx::wstring, boost::shared_ptr<RenderProcess>>> m_lpsspRenderProcess;
    };

    MARATHON_ASSERT_SIZEOF(RenderScheduler, 0x10);
}
