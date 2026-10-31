#pragma once

#include <Marathon.inl>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/DocState.h>
#include <Sonicteam/PictureTitle.h>

class StateDevTitle : public Sonicteam::DocState, public Sonicteam::System::Singleton<StateDevTitle, 0x82D35E78, Sonicteam::System::CreateStatic<StateDevTitle, 0x82161860>>
{
public:
    xpointer<Sonicteam::PictureTitle> m_pPictureTitle;
};

MARATHON_ASSERT_OFFSETOF(StateDevTitle, m_pPictureTitle, 0x04);
MARATHON_ASSERT_SIZEOF(StateDevTitle, 0x08);
