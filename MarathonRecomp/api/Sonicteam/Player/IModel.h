#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>

namespace Sonicteam::Player
{
    class IModel : public IPlugIn
    {
    public:
        struct Vftable : public IPlugIn::Vftable
        {
            be<uint32_t> fpGetAnimationID;
            be<uint32_t> fpGetAnimationID2;
            be<uint32_t> fpIsAnimationEnd;
            be<uint32_t> fpSetFunction;
            be<uint32_t> fpCaseUpdate;
            be<uint32_t> fpGetClump;
            be<uint32_t> fpUpdateTransform;
        };
    };

    MARATHON_ASSERT_OFFSETOF(IModel::Vftable, fpGetAnimationID, 4);
    MARATHON_ASSERT_OFFSETOF(IModel::Vftable, fpGetAnimationID2, 8);
    MARATHON_ASSERT_OFFSETOF(IModel::Vftable, fpIsAnimationEnd, 0xC);
    MARATHON_ASSERT_OFFSETOF(IModel::Vftable, fpSetFunction, 0x10);
    MARATHON_ASSERT_OFFSETOF(IModel::Vftable, fpCaseUpdate, 0x14);
    MARATHON_ASSERT_OFFSETOF(IModel::Vftable, fpGetClump, 0x18);
    MARATHON_ASSERT_OFFSETOF(IModel::Vftable, fpUpdateTransform, 0x1C);
    MARATHON_ASSERT_SIZEOF(IModel, 0x20);
}
