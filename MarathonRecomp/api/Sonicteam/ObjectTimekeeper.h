#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/PhaseShiftFilter.h>
#include <Sonicteam/PropFixture.h>
#include <Sonicteam/SceneLightParam.h>

namespace Sonicteam
{
    class ObjectTimekeeper : public PropFixture
    {
    public:
        class IUserLightImp : public SceneLightParam::IUserLight
        {
        public:
            SoX::LinkNode<IUserLightImp> m_Siblings;
            be<uint32_t> m_SiblingCount;
            MARATHON_INSERT_PADDING(0x04);
            boost::shared_ptr<SceneLightParam::AmbientLight> m_spAmbientLight;
            boost::shared_ptr<SceneLightParam::DirectionalLight> m_spMainLight;
            boost::shared_ptr<SceneLightParam::DirectionalLight> m_spSubLight;
        };

        be<uint32_t> m_Field180;
        be<float> m_LightTransitionStart;
        be<float> m_LightTransitionEnd;
        be<float> m_Time;
        boost::shared_ptr<SceneLightParam::DirectionalLight> m_spMainLight;
        boost::shared_ptr<SceneLightParam::DirectionalLight> m_spSubLight;
        boost::shared_ptr<SceneLightParam::AmbientLight> m_spAmbientLight;
        MARATHON_INSERT_PADDING(0x60);
        SoX::RefSharedPointer<IUserLightImp> m_spUserLight;
        SoX::RefSharedPointer<PhaseShiftFilter> m_spPhaseShiftFilter;
        MARATHON_INSERT_PADDING(0x70);
        be<uint32_t> m_Field280;
        uint8_t m_Field284;
        be<float> m_Field288;
    };

    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper::IUserLightImp, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper::IUserLightImp, m_SiblingCount, 0x14);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper::IUserLightImp, m_spAmbientLight, 0x1C);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper::IUserLightImp, m_spMainLight, 0x24);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper::IUserLightImp, m_spSubLight, 0x2C);
    MARATHON_ASSERT_SIZEOF(ObjectTimekeeper::IUserLightImp, 0x34);

    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_Field180, 0x180);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_LightTransitionStart, 0x184);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_LightTransitionEnd, 0x188);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_Time, 0x18C);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_spMainLight, 0x190);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_spSubLight, 0x198);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_spAmbientLight, 0x1A0);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_spUserLight, 0x208);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_spPhaseShiftFilter, 0x20C);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_Field280, 0x280);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_Field284, 0x284);
    MARATHON_ASSERT_OFFSETOF(ObjectTimekeeper, m_Field288, 0x288);
    MARATHON_ASSERT_SIZEOF(ObjectTimekeeper, 0x290);
}
