#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/SoX/Audio/Cue.h>
#include <Sonicteam/SoX/Audio/Listener.h>
#include <Sonicteam/SoX/Audio/ObjectManager.h>
#include <Sonicteam/SoX/Audio/SoundBank.h>
#include <Sonicteam/SoX/LinkNeighbor.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <stdx/compare/string_compare.h>
#include <stdx/map.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam::SoX::Audio
{
    class PlayerImpl : public Player
    {
    public:
        class DefaultManager : public ObjectManager {};

        xpointer<Listener> m_pListener;
        LinkNeighbor<Cue> m_Cues;
        MARATHON_INSERT_PADDING(0x0C);
        boost::shared_ptr<ObjectManager> m_spObjectManager;
        stdx::vector<RefSharedPointer<SoundBank>> m_SoundBanks;
        MARATHON_INSERT_PADDING(0x0C);
        stdx::map<stdx::string, be<uint32_t>, stdx::string_compare> m_SoundBankIDs;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_SIZEOF(PlayerImpl::DefaultManager, 0x04);

    MARATHON_ASSERT_OFFSETOF(PlayerImpl, m_pListener, 0x04);
    MARATHON_ASSERT_OFFSETOF(PlayerImpl, m_Cues, 0x08);
    MARATHON_ASSERT_OFFSETOF(PlayerImpl, m_spObjectManager, 0x1C);
    MARATHON_ASSERT_OFFSETOF(PlayerImpl, m_SoundBanks, 0x24);
    MARATHON_ASSERT_OFFSETOF(PlayerImpl, m_SoundBankIDs, 0x40);
    MARATHON_ASSERT_SIZEOF(PlayerImpl, 0x54);
}
