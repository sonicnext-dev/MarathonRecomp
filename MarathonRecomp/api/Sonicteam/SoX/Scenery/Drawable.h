#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/CameraEventCallback.h>
#include <Sonicteam/SoX/Graphics/FrameObserver.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Scenery
{
    class WorldImp;

    class Drawable : public RefCountObject, public CameraEventCallback
    {
    public:
        class frameObserver: SoX::Graphics::FrameObserver
        {
            xpointer<Drawable> m_pParent;
            MARATHON_INSERT_PADDING(0x28);
        };

        struct Vftable : RefCountObject::Vftable
        {
            be<uint32_t> fpUpdateRenderFlag;
            be<uint32_t> fpOutPosition; // -- ?
            be<uint32_t> fpOutTransform; // -- ?
            be<uint32_t> fpFunc10;
            be<uint32_t> fpUpdateInstance;
            be<uint32_t> fpGetDrawableNameWeird; // 
            be<uint32_t> fpGetDrawableName;
            be<uint32_t> fpReset; // should free from deque = no render
            be<uint32_t> fpFunc24;
        };

        void Reset()
        {
            MARATHON_CALL_VIRTUAL_FUNCTION(RefCountObject, void, &Vftable::fpReset, this);
        }

        xpointer<WorldImp> m_pWorld; 
        MARATHON_INSERT_PADDING(0x8);
        be<uint32_t> m_DrawableFlag;
        MARATHON_INSERT_PADDING(0x4);
        frameObserver m_frameObserver;
        LinkNode<Drawable> m_lnDrawable;
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(Drawable,m_pWorld, 0xC);
    MARATHON_ASSERT_OFFSETOF(Drawable, m_DrawableFlag, 0x18);
    MARATHON_ASSERT_OFFSETOF(Drawable, m_frameObserver, 0x20);
    MARATHON_ASSERT_OFFSETOF(Drawable, m_lnDrawable, 0x60);
    MARATHON_ASSERT_SIZEOF(Drawable, 0x70);
}
