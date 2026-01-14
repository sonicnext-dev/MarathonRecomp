#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackBase.h>
#include <Sonicteam/Spanverse/AcroartsBinary.h>

namespace Sonicteam::Spanverse
{
    class AckDataInst;
    struct _ackTrunk;
    // constructor 828F6A90 
    class _ackMomBase : public _ackBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x414B4D42; // AKMB
        xpointer<AckDataInst> m_pDataInstance;
        xpointer<AcroartsBinaryLeafMomNode> m_pBinaryMomNode;
        xpointer<AcroartsBinaryLeaf> m_pParentBinaryLeaf;
        be<uint32_t> m_Field1C; // -- ?
        be<float> m_Field20;
        MARATHON_INSERT_PADDING(4);
        xpointer<_ackTrunk> m_pTrunk;
        xpointer<void> m_pBranchat0x50;
        xpointer<void> m_pAckLeaf0x50;
        MARATHON_INSERT_PADDING(4);
        xpointer<AcroartsBinaryLeafMomValue> m_pBinaryMomData;
    };


    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_pDataInstance, 0x10);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_pBinaryMomNode, 0x14);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_pParentBinaryLeaf, 0x18);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_Field1C, 0x1C);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_Field20, 0x20);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_pTrunk, 0x28);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_pBranchat0x50, 0x2C);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_pAckLeaf0x50, 0x30);
    MARATHON_ASSERT_OFFSETOF(_ackMomBase, m_pBinaryMomData, 0x38);
    MARATHON_ASSERT_SIZEOF(_ackMomBase, 0x3C);
}
