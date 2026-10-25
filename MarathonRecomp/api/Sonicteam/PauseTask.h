#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/Audio/Player.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/HUDCALLBACK.h>
#include <Sonicteam/HUDPause.h>
#include <Sonicteam/TextBook.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextEntity.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class AssignedPauseMenuListener;

    class PauseTask : public SoX::Engine::Task, public HUDCALLBACK
    {
    public:
        enum PauseTaskState
        {
            PauseTaskState_Opened,
            PauseTaskState_Opening,
            PauseTaskState_Idle,
            PauseTaskState_ClosingToAction,
            PauseTaskState_Closing,
            PauseTaskState_Closed
        };

        xpointer<HUDPause> m_pHUDPause;
        be<float> m_TextPriority;
        MARATHON_INSERT_PADDING(0x14);
        be<PauseTaskState> m_State;
        be<uint32_t> m_Flags;
        be<uint32_t> m_SelectedIndex;
        be<uint32_t> m_Field78;
        MARATHON_INSERT_PADDING(0x04);
        boost::shared_ptr<AssignedPauseMenuListener> m_spAssignedPauseMenuListener;
        MARATHON_INSERT_PADDING(0x1C);
        SoX::Array<stdx::string, 16> m_ItemNames;
        SoX::Array<bool, 16> m_ItemEnabledFlags;
        boost::shared_ptr<TextCard> m_spMissionTextCard;
        boost::shared_ptr<TextEntity> m_spMissionTextEntity;
        be<uint32_t> m_ItemCount;
        boost::shared_ptr<TextCard> m_spPauseTextCard;
        SoX::Array<boost::shared_ptr<TextCard>, 16> m_ItemTextCards;
        MARATHON_INSERT_PADDING(0x98);
        boost::shared_ptr<SoX::Audio::Player> m_spAudioPlayer;
        MARATHON_INSERT_PADDING(0x04);

        bool Add(const char* in_pItemName, TextBook* in_pTextBook, const char* in_pTextKey, bool in_isEnabled = true)
        {
            return Insert(m_ItemCount, in_pItemName, in_pTextBook, in_pTextKey, in_isEnabled);
        }

        bool Insert(const uint32_t in_index, const char* in_pItemName, TextBook* in_pTextBook, const char* in_pTextKey, bool in_isEnabled = true)
        {
            const auto size = m_ItemCount;
            const auto capacity = m_ItemNames.Capacity();

            // Index exceeds capacity, or capacity has been reached.
            if (in_index >= capacity || size >= capacity)
                return false;

            const auto it = in_pTextBook->m_TextCards.find(in_pTextKey);

            if (it == in_pTextBook->m_TextCards.end())
                return false;

            auto index = in_index;

            // Insert at the end of the array, if
            // the index exceeds the current size.
            if (size < index)
                index = size;

            m_ItemNames.Insert(index, stdx::string(in_pItemName));
            m_ItemEnabledFlags.Insert(index, in_isEnabled);
            m_ItemTextCards.Insert(index, it->second);

            m_ItemCount++;

            return true;
        }

        bool RemoveAt(const uint32_t in_index)
        {
            const auto size = m_ItemCount;

            // Index is out of bounds, or there are no items to remove.
            if (in_index >= size || size == 0)
                return false;

            m_ItemNames.RemoveAt(in_index);
            m_ItemEnabledFlags.RemoveAt(in_index);
            m_ItemTextCards.RemoveAt(in_index);

            m_ItemCount--;

            return true;
        }

        bool Remove(const char* in_pItemName)
        {
            for (int i = 0; i < m_ItemNames.Capacity(); i++)
            {
                if (strcmp(m_ItemNames[i].c_str(), in_pItemName) == 0)
                    return RemoveAt(i);
            }

            return false;
        }
    };

    MARATHON_ASSERT_OFFSETOF(PauseTask, m_pHUDPause, 0x50);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_TextPriority, 0x54);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_State, 0x6C);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_Flags, 0x70);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_SelectedIndex, 0x74);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_Field78, 0x78);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_spAssignedPauseMenuListener, 0x80);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_ItemNames, 0xA4);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_ItemEnabledFlags, 0x264);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_spMissionTextCard, 0x274);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_spMissionTextEntity, 0x27C);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_ItemCount, 0x284);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_spPauseTextCard, 0x288);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_ItemTextCards, 0x290);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_spAudioPlayer, 0x3A8);
    MARATHON_ASSERT_SIZEOF(PauseTask, 0x3B4);
}
