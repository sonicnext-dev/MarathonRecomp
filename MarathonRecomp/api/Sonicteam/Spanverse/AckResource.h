#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/Spanverse/AckDataInst.h>

namespace Sonicteam::Spanverse
{
    class AckResourceMgr;

    class AckResourceData
    {
    public:
        be<uint16_t> m_Field00;
        be<uint16_t> m_Field02;
        be<uint32_t> m_Size;
        be<uint32_t> m_Field08;
        be<uint32_t> m_Field0C; 
        be<uint32_t> m_Field10;
        xpointer<void> m_Field14;
        xpointer<void> m_Field18;
        xpointer<void> m_Field1C;
        xpointer<void> m_Field20;
        xpointer<void> m_Field24;
    };

    class AckResource : public SoX::IResource2<AckResource, AckResourceMgr>
    {
    public:
        be<uint32_t> m_Index;
        xpointer<AckResourceData> m_pData;
        xpointer<void> m_pSpanverseFile;
        xpointer<AckDataInst> m_pABDTChunk;
        be<uint32_t> m_SpanIndex;
    };

    MARATHON_ASSERT_OFFSETOF(AckResource, m_Index, 0x64);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_pData, 0x68);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_pSpanverseFile, 0x6C);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_pABDTChunk, 0x70);
    MARATHON_ASSERT_OFFSETOF(AckResource, m_SpanIndex, 0x74);
}
