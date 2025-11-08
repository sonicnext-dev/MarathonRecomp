#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX::Engine
{
    class Doc;

    class GTask : public Component
    {
    public:
        be<uint32_t> m_Flags;
        xpointer<GTask> m_pPrevSibling;
        xpointer<GTask> m_pNextSibling;
        xpointer<GTask> m_pDependency;
        xpointer<GTask> m_pDependencies;

        GTask* GetFirstDependency() const
        {
            if (!m_pDependencies)
                return nullptr;

            auto pCurrent = m_pDependencies.get();

            while (pCurrent->m_pPrevSibling && pCurrent->m_pPrevSibling != m_pDependencies.get())
                pCurrent = pCurrent->m_pPrevSibling.get();

            return pCurrent;
        }

        GTask* GetLastDependency() const
        {
            return m_pDependencies.get();
        }
    };
}
