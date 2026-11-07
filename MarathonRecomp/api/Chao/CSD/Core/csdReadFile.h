#pragma once

#include <Marathon.inl>
#include <stdx/string.h>

namespace Chao::CSD
{
    class CReadFile
    {
    public:
        xpointer<void> m_pVftable;
        stdx::string m_FilePath;
    };

    MARATHON_ASSERT_OFFSETOF(CReadFile, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(CReadFile, m_FilePath, 0x04);
    MARATHON_ASSERT_SIZEOF(CReadFile, 0x20);
}
