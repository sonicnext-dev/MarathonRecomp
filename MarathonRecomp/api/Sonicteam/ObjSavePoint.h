#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/Instance.h>
#include <Sonicteam/PropFixture.h>
#include <Sonicteam/SavePointObjInfo.h>

namespace Sonicteam
{
    class ObjSavePoint : public PropFixture
    {
    public:
        xpointer<SavePointObjInfo> m_pObjInfo;
        MARATHON_INSERT_PADDING(0x60);
        xpointer<Prop::Instance> m_pPropInstance;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_OFFSETOF(ObjSavePoint, m_pObjInfo, 0x180);
    MARATHON_ASSERT_OFFSETOF(ObjSavePoint, m_pPropInstance, 0x1E4);
    MARATHON_ASSERT_SIZEOF(ObjSavePoint, 0x1F0);
}
