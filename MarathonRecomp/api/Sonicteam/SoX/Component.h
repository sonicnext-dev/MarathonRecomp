#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkedList.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::SoX
{
    class Component : public Object
    {
    public:
        struct Vftable : public Object::Vftable
        {
            be<uint32_t> fpDtor;
            be<uint32_t> fpUpdate;
        };

        xpointer<Component> m_pParent;
        LinkNode<Component> m_Siblings;
        LinkedList<Component> m_Children;

        template <typename T = Component>
        T* GetParent()
        {
            return (T*)m_pParent.get();
        }

        void Update(float in_deltaTime = 0.0f)
        {
            GuestToHostFunction<void>(((Vftable*)m_pVftable.get())->fpUpdate, this, in_deltaTime);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Component, m_pParent, 0x04);
    MARATHON_ASSERT_OFFSETOF(Component, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(Component, m_Children, 0x14);
    MARATHON_ASSERT_SIZEOF(Component, 0x20);
}
