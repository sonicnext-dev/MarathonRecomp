#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackBase.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Spanverse
{
    class SpkSpangleBase;
    class _ackMomBase;
    struct _ackTrunk;

    struct ackLeaf50
    {
        xpointer<SpkSpangleBase> pSpangleObject; // Object, CellSprite, Camera, Screen, Light, Primitive, Particle
        be<uint32_t> SpkSpangleBaseType;
        xpointer<AcroartsBinaryLeafData> pBinaryLeafResourceData;
        be<uint32_t> FieldC;
        be<uint32_t> Field10;
        be<uint32_t> Field14;
    };

    class ackLeaf : public _ackBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x414B4C46; // AKLF
		
        MARATHON_INSERT_PADDING(0x30);
        SoX::Math::Vector m_Position;
        ackLeaf50 m_LeafData;
        xpointer<AcroartsBinaryLeaf> m_pBinaryLeaf;
        xpointer<ackLeaf> m_pOtherLeaf1;
        xpointer<ackLeaf> m_pOtherLeaf2;
        MARATHON_INSERT_PADDING(0x18);
        xpointer<_ackTrunk> m_pTrunk;
        xpointer<void> m_pTransform; //ackBranch + 0x50 (Either To Matrix or struct at 0x50)
        xpointer<AckDataInst> m_pDataInstance;
        MARATHON_INSERT_PADDING(4);
        xpointer<_ackMomBase> m_pAckMomBase; // _acmParticlePlay _acm....
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_OFFSETOF(ackLeaf50, pSpangleObject, 0);
    MARATHON_ASSERT_OFFSETOF(ackLeaf50, SpkSpangleBaseType, 4);
    MARATHON_ASSERT_OFFSETOF(ackLeaf50, pBinaryLeafResourceData, 0x8);
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_LeafData, 0x50);
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_pBinaryLeaf, 0x68);
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_pOtherLeaf1, 0x6c);
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_pOtherLeaf2, 0x70);
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_pTrunk, 0x8C);
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_pTransform, 0x90); //Matrix4x4 ( + 0x50 Means Struct ehhh)
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_pDataInstance, 0x94);
    MARATHON_ASSERT_OFFSETOF(ackLeaf, m_pAckMomBase, 0x9C);
    MARATHON_ASSERT_SIZEOF(ackLeaf, 0xB0);
}
