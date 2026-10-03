#pragma once

namespace Sonicteam::SoX
{
    template <typename T>
    class ILinkNodeA
    {
    public:
        xpointer<T> m_pThisObject;
        xpointer<T> m_pPThread;
        xpointer<T> m_pNThread;
    };

    template <typename T>
    class LinkNodeA : public ILinkNodeA<LinkNodeA<T>>
    {
    public:
        xpointer<T> m_pTThread;
    };

    MARATHON_ASSERT_SIZEOF(LinkNodeA<uint32_t>, 0x10);
}
