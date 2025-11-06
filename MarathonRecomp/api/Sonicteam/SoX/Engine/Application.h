#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Engine
{
    class Application : Object
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<Doc> m_pDoc;
    };

    MARATHON_ASSERT_OFFSETOF(Application, m_pDoc, 0x08);
    MARATHON_ASSERT_SIZEOF(Application, 0x0C);
}
