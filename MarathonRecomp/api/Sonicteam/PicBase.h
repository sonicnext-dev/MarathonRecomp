#pragma once

#include <Marathon.inl>


namespace Sonicteam
{
    class PicBase
    {
    public:
        xpointer<void> m_pVftable;
        be<uint32_t> m_State1;
        be<uint32_t> m_State2; 
        be<uint32_t> m_State3;
    };
}
