#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdRCObject.h>
#include <Chao/CSD/Manager/csdmScene.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Input/Listener.h>
#include <Sonicteam/PicBase.h>

namespace Sonicteam
{
    class PictureTitle : public PicBase, public SoX::Input::Listener
    {
    public:
        enum PictureTitleFlags
        {
            PictureTitleFlags_Next = (1 << 0)
        };

        xpointer<SoX::Engine::Doc> m_pDoc;
        bool m_IsOutroFinished;
        be<uint32_t> m_Flags;
        xpointer<Chao::CSD::RCObject<Chao::CSD::CScene>> m_pBackAoitaScene;
        be<float> m_BackAoitaOutroSpeed;
        be<float> m_BackAoitaOutroEndFrame;
        xpointer<Chao::CSD::RCObject<Chao::CSD::CScene>> m_pTitleMenuScene;
        be<float> m_TitleMenuOutroSpeed;
        be<float> m_TitleMenuOutroEndFrame;
    };

    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_pDoc, 0x2C);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_IsOutroFinished, 0x30);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_Flags, 0x34);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_pBackAoitaScene, 0x38);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_BackAoitaOutroSpeed, 0x3C);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_BackAoitaOutroEndFrame, 0x40);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_pTitleMenuScene, 0x44);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_TitleMenuOutroSpeed, 0x48);
    MARATHON_ASSERT_OFFSETOF(PictureTitle, m_TitleMenuOutroEndFrame, 0x4C);
    MARATHON_ASSERT_SIZEOF(PictureTitle, 0x50);
}
