#pragma once

#include <Marathon.inl>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam
{
    class SceneryFactoryXenon : public System::Singleton<SceneryFactoryXenon, 0x82D3BC5C, System::CreateStatic<SceneryFactoryXenon, 0x825EB070>>
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x14);
        };

        xpointer<Vftable> m_pVftable;
    };
}