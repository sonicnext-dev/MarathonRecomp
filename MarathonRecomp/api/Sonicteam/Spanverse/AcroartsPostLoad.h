#pragma once

#include <Marathon.inl>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>
#include <api/stdx/map.h>
#include <api/stdx/string.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Spanverse
{
    class Env;
    class SpkSpangleServer;
    class SpkEssenceServer;

    class AcroartsPostLoad : public System::Singleton<AcroartsPostLoad, 0x82D3C4F0, System::CreateStatic<AcroartsPostLoad, 0x82642018>>
    {
    public:
        stdx::map<stdx::string, boost::shared_ptr<void>> m_mspField0;
        xpointer<Env> m_pEnv;
        MARATHON_INSERT_PADDING(0x8);
        xpointer<SpkSpangleServer> m_pSpanSSGS;
        xpointer<SpkEssenceServer> m_pSpanSSES;
    };

    MARATHON_ASSERT_OFFSETOF(AcroartsPostLoad, m_mspField0, 0);
    MARATHON_ASSERT_OFFSETOF(AcroartsPostLoad, m_pEnv, 0xC);
    MARATHON_ASSERT_OFFSETOF(AcroartsPostLoad, m_pSpanSSGS, 0x18);
    MARATHON_ASSERT_OFFSETOF(AcroartsPostLoad, m_pSpanSSES, 0x1C);

}
