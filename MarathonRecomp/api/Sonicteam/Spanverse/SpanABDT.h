#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpanACBS.h>
#include <Sonicteam/Spanverse/SpanBASE.h>

namespace Sonicteam::Spanverse
{
    class CustomEssenceTextureBase;

    class SpanABDT : public SpanACBS
    {
    public:
        struct Vftable: SpanACBS::Vftable
        {
            be<uint32_t> fpInitializeResource; // (index argument)
            be<uint32_t> fpDestroyResource; // (index argument)
        };
        xpointer<void> m_SectionABDA;
        xpointer<void> m_Node; // (version , then root node data, and others node)
        MARATHON_INSERT_PADDING(0xC);
        xpointer<SpanBASE> m_pSpanNode; // first, current, ?
        xpointer<xpointer<CustomEssenceTextureBase>> m_ppTexture;
        xpointer<void> m_pResourceIndices;
        xpointer<void> m_pResourceTable2;
        xpointer<void> m_pResourceTable3;
        be<uint32_t> m_ResourceCount; // ABRS
        be<uint32_t> m_InitializedResourceCount;
        be<uint32_t> m_Field40;
        be<uint32_t> m_InitializedResourceFlag;
    };

    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_pSpanNode, 0x24);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_ppTexture, 0x28);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_pResourceIndices, 0x2C);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_pResourceTable2, 0x30);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_pResourceTable3, 0x34);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_ResourceCount, 0x38);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_InitializedResourceCount, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(SpanABDT, m_InitializedResourceFlag, 0x44);
    MARATHON_ASSERT_SIZEOF(SpanABDT, 0x48);
}
