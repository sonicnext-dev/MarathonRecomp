#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::SoX::Engine
{
    class Application : public Object
    {
    public:
        bool m_Field04;
        xpointer<Doc> m_pDoc;
    };

    MARATHON_ASSERT_OFFSETOF(Application, m_Field04, 0x04);
    MARATHON_ASSERT_OFFSETOF(Application, m_pDoc, 0x08);
    MARATHON_ASSERT_SIZEOF(Application, 0x0C);
}
