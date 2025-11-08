#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::SoX
{
    class Component : public Object
    {
    public:
        struct Vftable : public Object::Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpUpdate;
        };

        xpointer<Component> m_pParent;
        LinkNode<Component> m_lnComponent;
        LinkedList<Component> m_llComponent;

        template <typename T = Component>
        T* GetParent()
        {
            return (T*)m_pParent.get();
        }

        void Update(float deltaTime)
        {
            GuestToHostFunction<void>(((Vftable*)m_pVftable.get())->fpUpdate, this, deltaTime);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Component, m_pParent, 0x04);
    MARATHON_ASSERT_SIZEOF(Component, 0x20);
}
