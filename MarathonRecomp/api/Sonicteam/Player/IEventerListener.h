#pragma once

namespace Sonicteam::Player
{
    class IEventerListener
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(8);
    };
}
