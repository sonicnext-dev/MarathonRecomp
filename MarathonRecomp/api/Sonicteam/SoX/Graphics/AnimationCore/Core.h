#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <stdx/string.h>

namespace Sonicteam::SoX::Graphics::AnimationCore
{
    class SubAnimation;
    class ConstRepeat; // IRepeat

    class Core
    {
    public:
        xpointer<void> m_pVftable;
        stdx::map<be<uint32_t>, boost::shared_ptr<SubAnimation>> m_mspSubAnimations;
        stdx::map<stdx::string, boost::shared_ptr<void*>> m_mspField10; //stdx::string, (not sure next), but 8 bytes left
        boost::shared_ptr<ConstRepeat> m_spCRepeat;
    };

    MARATHON_ASSERT_OFFSETOF(Core, m_pVftable, 0);
    MARATHON_ASSERT_OFFSETOF(Core, m_mspSubAnimations, 4);
    MARATHON_ASSERT_OFFSETOF(Core, m_mspField10, 0x10);
    MARATHON_ASSERT_OFFSETOF(Core, m_spCRepeat, 0x1C);
    MARATHON_ASSERT_SIZEOF(Core, 0x24);
}
