#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX::Engine
{
    class Doc;
    class Task : public Component, public MessageReceiver
    {
    public:
        be<uint32_t> m_flags; //used to store last pointer for itterate, sometimes
        be<uint32_t> m_timestamp; //not always
        xpointer<Task> m_pSiblingPrev;
        xpointer<Task> m_pSiblingNext;
        xpointer<Task> m_pDependency;
        xpointer<Task> m_pDependents;
        xpointer<Sonicteam::SoX::Engine::Doc> m_pDoc;
        SoX::LinkNode<Task> m_lnTask;

        Task* GetFirstDependent() const 
        {
            if (!m_pDependents) return nullptr;

            Task* current = m_pDependents.get();
            while (current->m_pSiblingPrev && current->m_pSiblingPrev != m_pDependents.get()) {
                current = current->m_pSiblingPrev.get();
            }
            return current;
        }

        Task* GetLastDependent() const {
            return m_pDependents.get();
        }

    };
}
