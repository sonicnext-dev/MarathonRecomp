#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ILoad.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <stdx/map.h>
#include <stdx/deque.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Player
{
    class AnimationScript;

    class Load : public ILoad
    {
    public:
        stdx::map<std::string, SoX::RefSharedPointer<Package>> m_spPackageBinary;
        stdx::map<std::string, boost::shared_ptr<Sonicteam::Player::AnimationScript>> m_spAnimationScript;
        stdx::deque<SoX::RefSharedPointer<Sonicteam::SoX::RefCountObject>> m_dspField38;
        xpointer<SoX::Engine::Doc> m_pDoc;
    };


    MARATHON_ASSERT_OFFSETOF(Load, m_spPackageBinary, 0x20);
    MARATHON_ASSERT_OFFSETOF(Load, m_spAnimationScript, 0x2C);
    MARATHON_ASSERT_OFFSETOF(Load, m_dspField38, 0x38);
    MARATHON_ASSERT_OFFSETOF(Load, m_pDoc, 0x4C);
    MARATHON_ASSERT_SIZEOF(Load, 0x50);
}
