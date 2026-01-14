#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackBase.h>
#include <Sonicteam/Spanverse/SpkEssenceBase.h>
#include <Sonicteam/Spanverse/AcroartsBinary.h>

namespace Sonicteam::Spanverse
{
    class CustomEssenceTextureBase;

    class AckDataInst : public _ackBase
    {
    public:
        struct Vftable : _ackBase::Vftable
        {
            be<uint32_t> fpInitialize;
            be<uint32_t> fpRelease;
        };

        xpointer<AcroartsBinaryNativeData> m_pBinaryArchive;
        xpointer<AcroartsBinaryTrunkRoot> m_pRoot; // or node?
        xpointer<AcroartsBinaryBranch> m_pCurrentBranch;  // ref to ABDTMoveNode(sub_828F6400) //00 00 01 05
        xpointer<AcroartsBinaryLeaf> m_pCurrentLeaf; //00 00 00 80
        xpointer<AcroartsBinaryLeafMomNode> m_pCurrentMom;
        xpointer<SpkBase> m_pSpanNode; //   xpointer<SpanSSES> m_pSpanSSES;
        xpointer<xpointer<SpkEssenceBase>> m_ppResources;
        xpointer<void> m_pResourceIndices;
        xpointer<void> m_pResourceTable2;
        xpointer<char> m_pResourceTableInit;
        be<uint32_t> m_ResourceCount;
        be<uint32_t> m_InitializedResourceCount;
        be<uint32_t> m_Field40;
        be<uint32_t> m_InitializedResourceFlag;
    };

    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pBinaryArchive, 0x10);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pRoot, 0x14);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pCurrentBranch, 0x18);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pCurrentLeaf, 0x1C);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pSpanNode, 0x24);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_ppResources, 0x28);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pResourceIndices, 0x2C);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pResourceTable2, 0x30);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_pResourceTableInit, 0x34);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_ResourceCount, 0x38);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_InitializedResourceCount, 0x3C);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(AckDataInst, m_InitializedResourceFlag, 0x44);
    MARATHON_ASSERT_SIZEOF(AckDataInst, 0x48);
}
