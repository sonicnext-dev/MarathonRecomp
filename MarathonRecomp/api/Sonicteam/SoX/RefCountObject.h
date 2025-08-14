#pragma once

namespace Sonicteam::SoX
{
    class RefCountObject
    {
    public:
        xpointer<void> m_pVftable;
        be<uint32_t> m_ReferenceCount;
    };
}
