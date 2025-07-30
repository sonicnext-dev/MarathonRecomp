#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Camera
{
    class Cameraman : Actor
    {
    public:
        MARATHON_INSERT_PADDING(0x28);
        boost::anonymous_shared_ptr m_spMyInputObj;
        MARATHON_INSERT_PADDING(0x34);
        be<float> m_FOV;
        MARATHON_INSERT_PADDING(0x04);
        boost::anonymous_shared_ptr m_spKynapseControl;
    };
}
