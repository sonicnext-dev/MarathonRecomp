#pragma once

#include <Marathon.inl>
#include <Sonicteam/DocMarathonState.h>

namespace Sonicteam
{
    class AppMarathon
    {
    public:
        MARATHON_INSERT_PADDING(0x180);
        xpointer<DocMarathonState> m_pDoc;
    };
}
