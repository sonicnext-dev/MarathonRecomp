#pragma once

#include <Marathon.inl>
#include <Sonicteam/ObjInfo.h>

namespace Sonicteam
{
    class SavePointObjInfo : public ObjInfo
    {
    public:
        MARATHON_INSERT_PADDING(0x20);
        be<uint32_t> m_Passed;
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_OFFSETOF(SavePointObjInfo, m_Passed, 0x48);
    MARATHON_ASSERT_SIZEOF(SavePointObjInfo, 0x50);
}
