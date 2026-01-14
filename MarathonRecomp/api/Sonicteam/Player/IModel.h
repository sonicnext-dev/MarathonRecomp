#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>

namespace Sonicteam::Player
{
    class IModel : public IPlugIn
    {
    public:
        struct Vftable:IPlugIn::Vftable
        {
            be<uint32_t> fpGetAnimationID;
            be<uint32_t> fpGetAnimationID2;
            be<uint32_t> fpIsAnimationEnd;
            be<uint32_t> fpSetFunction; // boost::fuction
            be<uint32_t> fpCaseUpdate;
            be<uint32_t> fpGetClump;
            be<uint32_t> fpUpdateTransform; // or updat in general
        };
    };

    MARATHON_ASSERT_SIZEOF(IModel, 0x20);
}
