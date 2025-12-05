#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::PackageModel
{
    class IAnimation 
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpSetKeyFrame1;
            be<uint32_t> fpSetKeyFrame2;
            be<uint32_t> fpGetKeyFrame;
            be<uint32_t> fpFunc10;
            be<uint32_t> fpFunc14;
            be<uint32_t> fpFunc18;
            be<uint32_t> fpFunc1C;
            be<uint32_t> fpFunc20;
            be<uint32_t> fpFunc24;
            be<uint32_t> fpFunc28;
        };

        void SetKeyFrame2(double frame)
        {
            MARATHON_CALL_VIRTUAL_FUNCTION(IAnimation, void, &Vftable::fpSetKeyFrame2, this, frame);
        }

        double GetKeyFrame()
        {
            return MARATHON_CALL_VIRTUAL_FUNCTION(IAnimation, double, &Vftable::fpGetKeyFrame, this);
        }

        xpointer<void> m_pVftable;
    };

}
