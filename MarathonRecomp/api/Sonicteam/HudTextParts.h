#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextEntity.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class HudTextParts : public SoX::RefCountObject
    {
    public:
        xpointer<HudTextParts> m_pNext;
        xpointer<DocMarathonState> m_pDoc;
        boost::shared_ptr<TextCard> m_spTextCard;
        boost::shared_ptr<TextEntity> m_spTextEntity;
        xpointer<CsdObject> m_pCsdObject;
        stdx::string m_SceneName;
        stdx::string m_CastName;
        be<float> m_Priority;
        be<float> m_OffsetX;
        be<float> m_OffsetY;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_AlignmentFlags;
    };
}
