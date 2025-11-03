#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/Spanverse/SpanABDT.h>

namespace Sonicteam::Spanverse
{
    class AckResourceMgr;

    //Converted .mab data
    class AckResourceA
    {
    public:
        be<uint16_t> m_Field0;
        be<uint16_t> m_Field2;
        be<uint32_t> m_Size;
        be<uint32_t> m_Field8; // = 4 like index or const?
        be<uint32_t> m_FieldC; 
        be<uint32_t> m_Field10;
        xpointer<void> m_Field14; //pointer to struct {ABDA OFFSET, ABRS OFFSET }
        xpointer<void> m_Field18; //pointer to EOFC (elements)  = EOFC + 0x10
        xpointer<void> m_Field1C; //pointer to EOFC (elements)  = EOFC + 0x14 WHY?
        xpointer<void> m_Field20; //pointer to EOFC (elements)  = EOFC + 0x14 WHY??
        xpointer<void> m_Field24; //pointer to EOFC (elements)  = EOFC + 0x14 WHY???
    };

    class AckResource : public SoX::IResource2<AckResource, AckResourceMgr>
    {
    public:
        be<uint32_t> m_Index;
        xpointer<AckResourceA> m_pAckResourceA;
        xpointer<void> m_pSpanverseFile; // SpanverseMemory to free
        xpointer<SpanABDT> m_pSpanABDT;
        be<uint32_t> m_SpanIndex;
    };

    MARATHON_ASSERT_OFFSETOF(AckResource, m_Index, 0x64);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_pAckResourceA, 0x68);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_pSpanverseFile, 0x6C);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_pSpanABDT, 0x70);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_SpanIndex, 0x74);
}
