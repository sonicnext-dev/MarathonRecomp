#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Math/Math.h>

namespace Sonicteam::SoX::Graphics
{
    class FrameSynchronizer;

    class Frame : public RefCountObject
    {
    public:
        be<uint32_t> m_Flag;
        xpointer<Frame> m_pOwner;
        xpointer<Frame> m_pParent;
        xpointer<Frame> m_pNextSibling;
        xpointer<Frame> m_pPrevSibling;
        xpointer<Frame> m_pFirstChild;
        LinkNode<Frame> m_llFrame; //list
        xpointer<LinkNode<Frame>> m_plnFrame;
        xpointer<FrameSynchronizer> m_pSynchronizer;
        LinkNode<Frame> m_lnFrame1;
        LinkNode<Frame> m_lnFrame2;

        struct Vftable:RefCountObject::Vftable
        {
            be<uint32_t> fpFunc4;
            be<uint32_t> fpUpdate;
            be<uint32_t> fpGetTransform1;
            be<uint32_t> fpGetTransform2;
            be<uint32_t> fpSetTransform;
            be<uint32_t> fpGetCollisionFlag;
            be<uint32_t> GetContactEntity;
            be<uint32_t> fpFunc20;
        };
        
        void SetTransform(SoX::Math::Matrix4x4* matrix);

        void SetTransform(SoX::Math::Matrix4x4& matrix);

        void UpdateTransform(SoX::Math::Matrix4x4& matrix);
    };

    MARATHON_ASSERT_SIZEOF(Frame,0x4C);
    MARATHON_ASSERT_OFFSETOF(Frame, m_Flag,8);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pOwner, 0xC);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pParent, 0x10);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pNextSibling, 0x14);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pPrevSibling, 0x18);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pFirstChild, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Frame, m_llFrame, 0x20);
    MARATHON_ASSERT_OFFSETOF(Frame, m_plnFrame, 0x2C);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pSynchronizer, 0x30);
    MARATHON_ASSERT_OFFSETOF(Frame, m_lnFrame1, 0x34);
    MARATHON_ASSERT_OFFSETOF(Frame, m_lnFrame2, 0x40);
}

#include "Frame.inl"
