#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam::SoX::Graphics::AnimationCore
{
    class IRepeat; // IRepeat

    class SubAnimation
    {
    public:
        xpointer<void> m_pVftable;
        be<float> m_FrameStart;
        be<float> m_FrameEnd;
        be<uint32_t> m_FieldC;
        boost::shared_ptr<IRepeat> m_spConstRepeat;
        xpointer<void> m_aAnimationData;
        stdx::vector<be<float>> m_vField1C;
        be<uint32_t> m_Count; // flag maybe?
        stdx::vector<be<float>> m_vKeyFramePoints;

        template <typename T>
        T* GetAnimationData()
        {
            return (T*)m_aAnimationData.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_pVftable, 0);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_FrameStart, 4);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_FrameEnd, 8);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_FieldC, 0xC);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_spConstRepeat, 0x10);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_aAnimationData, 0x18);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_vField1C, 0x1C);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_Count, 0x2C);
    MARATHON_ASSERT_OFFSETOF(SubAnimation, m_vKeyFramePoints, 0x30);
    MARATHON_ASSERT_SIZEOF(SubAnimation, 0x40);
}
