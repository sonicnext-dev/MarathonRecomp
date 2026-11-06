#pragma once

#include <Marathon.inl>
#include <Acroarts/kine/_ackMomBase.h>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/TextEntity.h>

namespace Sonicteam::Spanverse
{
    class alignas(16) _acmSubtitle : public _ackMomBase
    {
    public:
        boost::shared_ptr<TextEntity> m_spTextEntity;
    };

    MARATHON_ASSERT_OFFSETOF(_acmSubtitle, m_spTextEntity, 0x3C);
    MARATHON_ASSERT_SIZEOF(_acmSubtitle, 0x50);
}
