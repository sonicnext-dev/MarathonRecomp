#pragma once

#include <Marathon.inl>

namespace Sonicteam::Spanverse
{
    struct AcroartsBinaryLeafMomValue;
    struct AcroartsBinaryLeafMomNode;
    struct AcroartsBinaryLeafMomRoot;
    struct AcroartsBinaryLeafData;
    struct AcroartsBinaryLeaf;
    struct AcroartsBinaryBranch;
    struct AcroartsBinaryTrunkRoot;
    struct AcroartsBinaryDataTrunk;
    struct AcroartsBinaryNativeData;


    struct AcroartsBinaryLeafMomValue
    {
        union
        {
            xpointer<const char> c_str;
            be<uint32_t> u32;
            be<float> f32;
        };
    };

    struct AcroartsBinaryLeafMomNode
    {
        be<uint32_t> Header;
        be<uint32_t> MomBaseID; // ref to MomServer::MomBase enum
        MARATHON_INSERT_PADDING(0xC);
        be<uint32_t> Count; // not always match
        xpointer<AcroartsBinaryLeafMomValue> MomData;
    };

    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomNode, Header, 0x0);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomNode, MomBaseID, 0x4);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomNode, Count, 0x14);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomNode, MomData, 0x18);

    struct AcroartsBinaryLeafMomRoot
    {
        be<uint32_t> Field0;
        be<uint32_t> Index;
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> Count; //0x18
        xpointer<xpointer<AcroartsBinaryLeafMomNode>> Children; //0x1C
    };

    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomRoot, Field0, 0x0);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomRoot, Index, 0x4);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomRoot, Count, 0x18);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeafMomRoot, Children, 0x1C);

    struct AcroartsBinaryLeafData
    {
        be<uint32_t> ResourceCount;
        xpointer<xpointer<uint32_t>> ResourceIndices;
        be<uint32_t> ResourceCount2;
        xpointer<xpointer<uint32_t>> ResourceIndices2; // CellSPrite do (*v6->Data.ResourceIndices2)[0] and *(a1->m_pBinaryLeaf->Data.ResourceIndices2)[1]
        be<uint32_t> ResourceCount3;
        xpointer<xpointer<uint32_t>> ResourceIndices3;
        be<uint32_t> ResourceCount4;
        xpointer<xpointer<uint32_t>> ResourceIndices4;
    };

    struct AcroartsBinaryLeaf
    {
        be<uint32_t> Header;
        MARATHON_INSERT_PADDING(0x80);
        be<uint32_t> SpkSpangleBaseType; //SpkSpangleServer::SpkSpangleBase
        AcroartsBinaryLeafData Data;
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> MomRootCount; //B8
        xpointer<xpointer<AcroartsBinaryLeafMomRoot>> MomRoots; //BC
    };

    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeaf, MomRootCount, 0xB8);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryLeaf, MomRoots, 0xBC);

    struct AcroartsBinaryBranch
    {
        be<uint32_t> Header;        
        be<int32_t> Field4;         
        be<float> Field8;           
        be<float> FieldC;           
        be<float> Field10;            
        be<float> Field14;            
        be<float> Flags;                 // Example ThunderShild ( == 0 shield move to default position where it was first initialized) == 1 (Normal Behaviour) , == 2(Attached i guess) ref to 828F9428
        be<uint32_t> AttachmentIndex;     // 0x1C: 0 (offset to child pointer array)
        be<int32_t> i2;              
        be<uint32_t> zero1;          // 0x24: 0 (ref to 828F9428)
        be<uint32_t> zero2;          // 0x28: 0 // Somethin (ref to 828F9428)
        be<int32_t> i3;            
        be<int32_t> i4;            
        be<uint32_t> zero3;        
        be<uint32_t> zero4;        
        be<uint32_t> zero5;        
        be<uint32_t> flags;        
        be<uint32_t> LeafsCount;   
        xpointer<xpointer<AcroartsBinaryLeaf>> ppLeafs;       // 0x48: 0x05B5DE20
    };
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryBranch, LeafsCount, 0x44);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryBranch, ppLeafs, 0x48);

    // ABDT  + 0x10
    struct AcroartsBinaryTrunkRoot // RootTrunk
    {
        be<uint32_t> Version; // 2006020901
        be<uint32_t> Header; // 0x40
        be<float> f1, f2, f4, f5, f6, f7, f8; // +-
        be<uint32_t> BranchesCount;
        xpointer<xpointer<AcroartsBinaryBranch>> ppBranches;
    };
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryTrunkRoot, BranchesCount, 0x24);
    MARATHON_ASSERT_OFFSETOF(AcroartsBinaryTrunkRoot, ppBranches, 0x28);


    // ABDT 
    struct AcroartsBinaryDataTrunk
    {
        be<uint32_t> Header;
        MARATHON_INSERT_PADDING(0x4);
        be<uint32_t> Offset;
        be<uint32_t> Flags; // Used For Initilization ( in binary should be 0)

        AcroartsBinaryTrunkRoot* GetRoot()
        {
            return (AcroartsBinaryTrunkRoot*)(((char*)this) + Offset);
        }
    };

    struct AcroartsBinaryNativeData //ABDA
    {
        be<uint32_t> Header;
        MARATHON_INSERT_PADDING(0x8);
        be<uint32_t> Flags; // Used For Initilization ( in binary should be 0)
        be<uint32_t> Version; //2006020901
        be<uint32_t> Count;
        MARATHON_INSERT_PADDING(0x8);
        union
        {
            xpointer<AcroartsBinaryDataTrunk> ptr;
            be<uint64_t> offset;
        } ppBinaryData[0];
    };
}
