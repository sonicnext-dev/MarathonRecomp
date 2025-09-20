#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam::SoX::Engine
{
    class Doc;

    class Task : public Component, public MessageReceiver
    {
    public:
        be<uint32_t> m_Flags;
        be<uint32_t> m_Timestamp;
        xpointer<Task> m_pPrevSibling;
        xpointer<Task> m_pNextSibling;
        xpointer<Task> m_pDependency;
        xpointer<Task> m_pDependencies;
        xpointer<Doc> m_pDoc;
        LinkNode<Task> m_lnTask;

        Task* GetFirstDependency() const 
        {
            if (!m_pDependencies)
                return nullptr;

            auto pCurrent = m_pDependencies.get();

            while (pCurrent->m_pPrevSibling && pCurrent->m_pPrevSibling != m_pDependencies.get())
                pCurrent = pCurrent->m_pPrevSibling.get();

            return pCurrent;
        }

        Task* GetLastDependency() const
        {
            return m_pDependencies.get();
        }
    };
}
