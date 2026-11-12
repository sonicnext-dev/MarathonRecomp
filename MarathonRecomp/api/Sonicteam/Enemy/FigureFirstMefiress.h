#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyTexture.h>
#include <Sonicteam/MyRenderProcessListener.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::Enemy
{
    class FigureFirstMefiress : public FigureEffect
    {
    public:
        MARATHON_INSERT_PADDING(0x20);
        SoX::RefSharedPointer<MyTexture> m_spUserTexture;
        MARATHON_INSERT_PADDING(0x6C);

        class MyRenderProcessListener : public Sonicteam::MyRenderProcessListener
        {
        public:
            xpointer<FigureFirstMefiress> m_pFigure;
            MARATHON_INSERT_PADDING(0x4C);
        };
    };

    MARATHON_ASSERT_OFFSETOF(FigureFirstMefiress::MyRenderProcessListener, m_pFigure, 0x10);
    MARATHON_ASSERT_SIZEOF(FigureFirstMefiress::MyRenderProcessListener, 0x60);

    MARATHON_ASSERT_OFFSETOF(FigureFirstMefiress, m_spUserTexture, 0xB0);
    MARATHON_ASSERT_SIZEOF(FigureFirstMefiress, 0x120);
}
