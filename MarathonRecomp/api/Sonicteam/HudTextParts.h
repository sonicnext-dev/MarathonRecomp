#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/TextCard.h>
#include <Sonicteam/TextEntity.h>

namespace Sonicteam
{
    class HudTextParts : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(8);
        boost::shared_ptr<TextCard> m_spTextCard;
        boost::shared_ptr<TextEntity> m_spTextEntity;
        xpointer<CsdObject> m_pCsdObject;
        MARATHON_INSERT_PADDING(0x38);
        be<float> m_Priority;
        be<float> m_OffsetX;
        be<float> m_OffsetY;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_AlignmentFlags;
        MARATHON_INSERT_PADDING(0x0C);
        be<float> m_Field7C;
        be<float> m_Field80;
    };

    MARATHON_ASSERT_OFFSETOF(HudTextParts, m_Field80, 0x80);
}
