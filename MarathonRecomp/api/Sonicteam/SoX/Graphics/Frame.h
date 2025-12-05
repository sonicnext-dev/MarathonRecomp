#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Math/Math.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/Graphics/Frame.h>
#include <Sonicteam/SoX/Graphics/FrameObserver.h>

namespace Sonicteam::SoX::Graphics
{
    class Frame;

    class FrameSynchronizer
    {
    public:
        xpointer<void> m_pVftable;
        SoX::LinkedList<Frame> m_llFrames;
        SoX::ArrayOld<SoX::LinkedList<Frame>> m_aollFrame;
        xpointer<be<uint32_t>> m_aPriorityCounts;
        be<uint32_t> m_Count;

        FrameSynchronizer(uint32_t count)
        {
            m_pVftable = nullptr;
            m_llFrames.m_pPrev = &m_llFrames;
            m_llFrames.m_pNext = &m_llFrames;

            // Allocate arrays based on count
            m_aollFrame = SoX::ArrayOld<SoX::LinkedList<Frame>>(count);
            m_aPriorityCounts = (be<uint32_t>*)g_userHeap.Alloc(sizeof(be<uint32_t>) * count);

            // Initialize priority array to 0
            for (uint32_t i = 0; i < count; ++i) {
                m_aPriorityCounts[i] = 0;
            }

            m_Count = count;
        }

        void RemoveFromPriorityQueue(Frame* pFrame);
    };

    MARATHON_ASSERT_OFFSETOF(FrameSynchronizer, m_llFrames, 4);
    MARATHON_ASSERT_OFFSETOF(FrameSynchronizer, m_aollFrame, 0x10);
    MARATHON_ASSERT_OFFSETOF(FrameSynchronizer, m_aPriorityCounts, 0x14);
    MARATHON_ASSERT_OFFSETOF(FrameSynchronizer, m_Count, 0x18);

    class Frame : public RefCountObject
    {
    public:
        enum FrameFlags {
            FLAG_ACTIVE = 0x1,
            FLAG_VISIBLE = 0x2,
            FLAG_SYNC_ENABLED = 0x10,
            FLAG_DIRTY = 0x20,
            FLAG_IN_SYNC_LIST = 0x40
        };

        be<uint32_t> m_Flag;
        xpointer<Frame> m_pOwner;
        xpointer<Frame> m_pParent;
        xpointer<Frame> m_pNextSibling;
        xpointer<Frame> m_pPrevSibling;
        xpointer<Frame> m_pFirstChild;
        LinkNode<FrameObserver> m_llFrameObserver; // For Particles
        xpointer<LinkNode<Frame>> m_plnFrame;
        xpointer<FrameSynchronizer> m_pSynchronizer;
        LinkNode<Frame> m_lnFrame1;
        LinkNode<Frame> m_lnFrame2;

        struct Vftable : public RefCountObject::Vftable
        {
            be<uint32_t> fpFunc4;
            be<uint32_t> fpGetName;
            be<uint32_t> fpGetTransform1;
            be<uint32_t> fpGetTransform2;
            be<uint32_t> fpSetTransform;
            be<uint32_t> fpGetCollisionFlag;
            be<uint32_t> GetContactEntity;
            be<uint32_t> fpFunc20;
        };

        // Core hierarchy methods
        void AddChild(Frame* pChild);
        void RemoveChild(Frame* pChild);
        void RemoveFromParent();
        void DestroyFrame();
        bool Func8();

        // Transform methods
        void SetTransform(SoX::Math::Matrix4x4* matrix);
        void SetTransform(SoX::Math::Matrix4x4& matrix);
        void UpdateTransform(SoX::Math::Matrix4x4& matrix);

        // Utility methods
        bool IsAttached() const { return m_pParent != nullptr; }
        bool HasChildren() const { return m_pFirstChild != nullptr; }
        xpointer<Frame> GetRoot();
        void SetSynchronizer(FrameSynchronizer* pSync);

        void UpdateOwnerRecursive(Frame* pNewOwner);
        void DestroyChildren();

        void DetachFromParent();
        void DestroyFrameHierarchy();

        // Search and traversal methods
        const char* GetName();
        void ForEach(std::function<void(Frame*)>);
        Frame* FindNode(const char* name);

        Frame* FindChild(const char* name);
        Frame* FindNode(Frame* pFrame);
        bool ContainsFrame(Frame* pFrame);
        bool HasDirectChild(Frame* pFrame);
        void RemoveChildByName(const char* name);
        void RemoveNodeByName(const char* name);
    };

    MARATHON_ASSERT_SIZEOF(Frame, 0x4C);
    MARATHON_ASSERT_OFFSETOF(Frame, m_Flag, 8);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pOwner, 0xC);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pParent, 0x10);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pNextSibling, 0x14);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pPrevSibling, 0x18);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pFirstChild, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Frame, m_llFrameObserver, 0x20);
    MARATHON_ASSERT_OFFSETOF(Frame, m_plnFrame, 0x2C);
    MARATHON_ASSERT_OFFSETOF(Frame, m_pSynchronizer, 0x30);
    MARATHON_ASSERT_OFFSETOF(Frame, m_lnFrame1, 0x34);
    MARATHON_ASSERT_OFFSETOF(Frame, m_lnFrame2, 0x40);
}

#include "Frame.inl"
