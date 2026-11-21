#pragma once

#include <Marathon.inl>
#include <Kaim/CArray.h>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CData : public CObject
    {
    public:
        class CTypeInfo : public CObject
        {
        public:
            xpointer<char> m_name;
            xpointer<char> m_value;
        };

        be<uint32_t> m_memSize;
        xpointer<void> m_dataAccessor;
        xpointer<char> m_name;
        xpointer<char> m_type;
        xpointer<char> m_value;
        xpointer<xpointer<CData>> m_children;
        be<uint32_t> m_childrenSize;
        be<uint32_t> m_childCount;
        CArray<CTypeInfo> m_typeInfos;
    };

    MARATHON_ASSERT_OFFSETOF(CData::CTypeInfo, m_name, 0x04);
    MARATHON_ASSERT_OFFSETOF(CData::CTypeInfo, m_value, 0x08);
    MARATHON_ASSERT_SIZEOF(CData::CTypeInfo, 0x0C);

    MARATHON_ASSERT_OFFSETOF(CData, m_memSize, 0x04);
    MARATHON_ASSERT_OFFSETOF(CData, m_dataAccessor, 0x08);
    MARATHON_ASSERT_OFFSETOF(CData, m_name, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CData, m_type, 0x10);
    MARATHON_ASSERT_OFFSETOF(CData, m_value, 0x14);
    MARATHON_ASSERT_OFFSETOF(CData, m_children, 0x18);
    MARATHON_ASSERT_OFFSETOF(CData, m_childrenSize, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CData, m_childCount, 0x20);
    MARATHON_ASSERT_OFFSETOF(CData, m_typeInfos, 0x24);
    MARATHON_ASSERT_SIZEOF(CData, 0x34);
}
