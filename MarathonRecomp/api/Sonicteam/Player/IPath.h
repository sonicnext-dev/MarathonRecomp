#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IForce.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/PathAnimation/Controller.h>
#include <api/boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Player
{
    class IPath : public IPlugIn, public IPosturePlugIn, public IFlagCommunicator, public IStepable, public IDynamicLink, public IVariable
    {
    public:
        be<uint32_t> m_ICommonContextIFFlag;
        be<uint32_t> m_PostureRequestFlag;
        be<float> m_DirectionForward;
        be<float> m_DirectionVertical;
        SoX::RefSharedPointer<PathAnimation::Controller> m_spController;
        boost::shared_ptr<IForce> m_spGravityForce;
    };

    MARATHON_ASSERT_SIZEOF(IPath, 0x50);
    MARATHON_ASSERT_OFFSETOF(IPath, m_ICommonContextIFFlag, 0x34);
    MARATHON_ASSERT_OFFSETOF(IPath, m_PostureRequestFlag, 0x38);
    MARATHON_ASSERT_OFFSETOF(IPath, m_DirectionForward, 0x3C);
    MARATHON_ASSERT_OFFSETOF(IPath, m_DirectionVertical, 0x40);
    MARATHON_ASSERT_OFFSETOF(IPath, m_spController, 0x44);
    MARATHON_ASSERT_OFFSETOF(IPath, m_spGravityForce, 0x48);
}
