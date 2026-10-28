#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Input/Listener.h>
#include <Sonicteam/DocMarathonState.h>
#include <Sonicteam/PicBase.h>

namespace Sonicteam
{
    class PictureTitle : public PicBase, public SoX::Input::Listener
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
        xpointer<DocMarathonState> m_pDoc;
        MARATHON_INSERT_PADDING(0x20);
    };

    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_pDoc, 0x2C);
    MARATHON_ASSERT_SIZEOF(PictureTitle, 0x50);
}
