#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackBase.h>

namespace Sonicteam::Spanverse
{
    class ackBranch; //AKBR
    class AckDataInst;
    class AckMomServer;
    class SpangleObject;
    

    struct _ackTrunk
    {
        MARATHON_INSERT_PADDING(0x80);
        xpointer<AckDataInst> m_pDataInstance;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<AckMomServer> m_pMomServer;
        xpointer<SpangleObject> m_pSpangleObject;
        MARATHON_INSERT_PADDING(0x28);
        be<uint32_t> m_Flags; // (AcroartsRoot::Header >> 5) & 1
        MARATHON_INSERT_PADDING(0x14);
        xpointer<xpointer<ackBranch>> m_ppBranches;
        be<uint32_t> m_BranchCount;
        xpointer<void> m_pFieldD8; // 0x50 * count (array)
        MARATHON_INSERT_PADDING(0x8);
    };

    class AckTrunk : public _ackBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x414B5452; // AKTR

        struct Vftable : _ackBase::Vftable
        {
            be<uint32_t> fpFunc4;
            be<uint32_t> fpFunc8;
            be<uint32_t> fpFuncC;
            be<uint32_t> fpFunc10;
            be<uint32_t> fpFunc14;
            be<uint32_t> fpFunc18;
        };

        be<uint32_t> m_BinaryArchiveIndex;
        xpointer<AcroartsBinaryTrunkRoot> m_pRoot;
        MARATHON_INSERT_PADDING(0x8);
        _ackTrunk m_Data;   
        be<float> m_Field104;   
        MARATHON_INSERT_PADDING(0xC);
    };

    MARATHON_ASSERT_OFFSETOF(_ackTrunk, m_pDataInstance, 0x80);
    MARATHON_ASSERT_OFFSETOF(_ackTrunk, m_pMomServer, 0x88);
    MARATHON_ASSERT_OFFSETOF(_ackTrunk, m_pSpangleObject, 0x8C);
    MARATHON_ASSERT_OFFSETOF(_ackTrunk, m_Flags, 0xB8);
    MARATHON_ASSERT_OFFSETOF(_ackTrunk, m_ppBranches, 0xD0);
    MARATHON_ASSERT_OFFSETOF(_ackTrunk, m_BranchCount, 0xD4);
    MARATHON_ASSERT_OFFSETOF(_ackTrunk, m_pFieldD8, 0xD8);
    MARATHON_ASSERT_OFFSETOF(AckTrunk, m_BinaryArchiveIndex, 0x10);
    MARATHON_ASSERT_OFFSETOF(AckTrunk, m_pRoot, 0x14);
    MARATHON_ASSERT_OFFSETOF(AckTrunk, m_Data, 0x20);
    MARATHON_ASSERT_OFFSETOF(AckTrunk, m_Field104, 0x104);
    //MARATHON_ASSERT_SIZEOF(AckTrunk, 0x120);
}
