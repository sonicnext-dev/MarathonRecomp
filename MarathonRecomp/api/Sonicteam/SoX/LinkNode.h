#pragma once

namespace Sonicteam::SoX
{
    template <typename T>
    class ILinkNode
    {
    public:
        xpointer<T> m_pPrev;
        xpointer<T> m_pNext;
    };

    template <typename T>
    class LinkNode : public ILinkNode<LinkNode<T>>
    {
    public:
        xpointer<T> m_pThis;
    };

    template <typename T>
    class LinkRef
    {
    public:
        xpointer<T> m_pElement;
        LinkNode<LinkRef> m_lnElement;
    };
}
