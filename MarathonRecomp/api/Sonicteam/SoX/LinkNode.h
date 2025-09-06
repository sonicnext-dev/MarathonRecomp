#pragma once

namespace Sonicteam::SoX
{
    template <typename T>
    class LinkNodeTemplate
    {
    public:
        xpointer<T> m_pPrev;
        xpointer<T> m_pNext;
    };

    template <typename T>
    class LinkNode : public LinkNodeTemplate<LinkNode<T>>
    {
    public:
        xpointer<LinkNode<T>> m_pThis;
    };

    template <typename T>
    class LinkRef
    {
    public:
        xpointer<T> m_pElement;
        SoX::LinkNode<LinkRef> m_lnElement;
    };
}
