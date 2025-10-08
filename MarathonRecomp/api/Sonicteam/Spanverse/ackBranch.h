#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackBase.h>
#include <Sonicteam/SoX/Math/Matrix.h>

namespace Sonicteam::Spanverse
{
    class SpangleObject;
    struct _ackTrunk;
    struct AcroartsBinaryBranch;
    class AckDataInst;
    class ackLeaf;

    struct ackLeafData
    {
        be<uint32_t> Field0;
        be<uint32_t> Field4;
        be<uint32_t> Flags;
    };

    class ackBranch : public _ackBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x414B4252; // AKBR
		
        SoX::Math::Matrix4x4 m_Transform1;
        SoX::Math::Matrix4x4 m_Transform2;
        MARATHON_INSERT_PADDING(0x14);
        xpointer<SpangleObject> m_pSpangleObject;
        MARATHON_INSERT_PADDING(0x8);
        xpointer<_ackTrunk> m_pTrunkData;
        xpointer<AckDataInst> m_pAckDataInstance;
        xpointer<AcroartsBinaryBranch> m_pAcroartsB;
        xpointer<xpointer<ackBranch>> m_ppBranches; //
        xpointer<xpointer<ackLeaf>> m_ppLeafs; //
        be<uint32_t> m_FlagsC4; //Count
        MARATHON_INSERT_PADDING(0xC);
        be<uint32_t> m_LeafCount;
        xpointer<ackLeafData> m_LeafData; // something for leaf 
        
    };

    MARATHON_ASSERT_OFFSETOF(ackBranch, m_Transform1, 0x10);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_Transform2, 0x50);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_pSpangleObject, 0xA4);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_pTrunkData, 0xB0);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_pAckDataInstance, 0xB4);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_pAcroartsB, 0xB8);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_ppBranches, 0xBC);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_ppLeafs, 0xC0);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_FlagsC4, 0xC4);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_LeafCount, 0xD4);
    MARATHON_ASSERT_OFFSETOF(ackBranch, m_LeafData, 0xD8);
    MARATHON_ASSERT_SIZEOF(ackBranch, 0xE0);
}
