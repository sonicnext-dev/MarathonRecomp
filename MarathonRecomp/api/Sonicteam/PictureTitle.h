#pragma once

#include <Marathon.inl>
#include <Sonicteam/PicBase.h>
#include <Sonicteam/SoX/Input/Listener.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/DocMarathonState.h>

namespace Sonicteam
{
    class PictureTitle:public PicBase, public SoX::Input::Listener
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<Sonicteam::DocMarathonState> m_Doc;
        MARATHON_INSERT_PADDING(0x18);
    };
}
