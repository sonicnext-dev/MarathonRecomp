#pragma once

#include <Marathon.inl>
#include <stdx/map.h>
#include <stdx/string.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Spanverse
{
    //ModelPlugin::ModelInfo
    //ModelPlugin::ModelStandardInfo
    //ModelPlugin::ModelMorphInfo

    class EnvInfo
    {
    public:
        stdx::map<stdx::string, boost::shared_ptr<void>> m_mspModelInfo;
        boost::shared_ptr<void> m_spModelStandardInfo;
        boost::shared_ptr<void> m_spModelMorphInfo;
    };

    MARATHON_ASSERT_OFFSETOF(EnvInfo,m_mspModelInfo, 0x0);
    MARATHON_ASSERT_OFFSETOF(EnvInfo,m_spModelStandardInfo, 0xC);
    MARATHON_ASSERT_OFFSETOF(EnvInfo,m_spModelMorphInfo, 0x14);
    MARATHON_ASSERT_SIZEOF(EnvInfo, 0x1C);
}
