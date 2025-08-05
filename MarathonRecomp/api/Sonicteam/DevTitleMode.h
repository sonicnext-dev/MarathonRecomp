#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <Sonicteam/PictureTitle.h>
#include <Sonicteam/DocState.h>

//anonymous namespace
namespace
{
    class StateDevTitle : public Sonicteam::DocState
    {
    public:
        xpointer<Sonicteam::PictureTitle> m_PictureTitle;
    };
}
namespace Sonicteam
{
    class DevTitleMode : public SoX::Engine::DocMode
    {
        struct Mode {
            xpointer<const char> m_Name;
            MARATHON_INSERT_PADDING(4);
        };
    public:
        be<uint32_t> m_Selected;
        MARATHON_INSERT_PADDING(4);
        MARATHON_INSERT_PADDING(10); //stdx::vector<Mode> m_Mode;
    };
}
