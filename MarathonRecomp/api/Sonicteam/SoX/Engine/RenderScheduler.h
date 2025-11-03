#pragma once

#include <Marathon.inl>
#include <stdx/list.h>
#include <stdx/string.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::SoX::Engine
{
    class RenderProcess;

    class RenderScheduler
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFunc4;
        };
        xpointer<Vftable> m_pVftable;
        stdx::list<std::pair<stdx::string,boost::shared_ptr<RenderProcess>>> m_lpsspRenderProcess;
    };
}
