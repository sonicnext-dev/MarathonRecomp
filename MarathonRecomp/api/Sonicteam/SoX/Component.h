#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX
{
    namespace Engine
    {
        class DocMode;
    }

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

        template <typename T = Engine::DocMode>
        T* GetParent()
        {
            return (T*)m_pParent.get();
        }

        void Update(float delta)
        {
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpUpdate, this,delta);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Component, m_pParent, 0x04);
    MARATHON_ASSERT_SIZEOF(Component, 0x20);
}
