#pragma once

#include <Marathon.inl>
#include <Kaim/CEntity.h>
#include <Kaim/CGraphManager.h>
#include <Kaim/CPathObject.h>
#include <Kaim/CPointPair.h>
#include <Kaim/CVarList.h>
#include <Kaim/CWorldService.h>

namespace Kaim
{
    class CPathObjectManager : public CWorldService
    {
    public:
        struct PathObjectCallBack
        {
            xpointer<void> m_func;
            xpointer<void> m_param;
            xpointer<CEntity> m_entity;
        };

        xpointer<xpointer<CPathObject>> m_pathObjectList;
        be<uint32_t> m_nbObjects;
        xpointer<CGraphManager> m_graphMgr;
        be<uint32_t> m_paramMaxNbPathObjects;
        xpointer<CPointPair> m_keyPointsPairs;
        be<uint32_t> m_nbKeyPointPair;
        be<uint32_t> m_callbackSize;
        xpointer<CVarList<PathObjectCallBack>> m_callbacks;
    };

    MARATHON_ASSERT_OFFSETOF(CPathObjectManager::PathObjectCallBack, m_func, 0x00);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager::PathObjectCallBack, m_param, 0x04);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager::PathObjectCallBack, m_entity, 0x08);
    MARATHON_ASSERT_SIZEOF(CPathObjectManager::PathObjectCallBack, 0x0C);

    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_pathObjectList, 0x04);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_nbObjects, 0x08);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_graphMgr, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_paramMaxNbPathObjects, 0x10);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_keyPointsPairs, 0x14);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_nbKeyPointPair, 0x18);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_callbackSize, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CPathObjectManager, m_callbacks, 0x20);
    MARATHON_ASSERT_SIZEOF(CPathObjectManager, 0x24);
}
