#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>

namespace Sonicteam::SoX::Engine
{
    class Doc
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(4);
        xpointer<DocMode> m_pDocMode;
        xpointer<Task> m_pRootTask;
        xpointer<Task> m_pRootGTask;
        MARATHON_INSERT_PADDING(8);
        xpointer<Task> m_pDocModeExecutor;
        MARATHON_INSERT_PADDING(0x3C);

        template <typename T = DocMode>
        inline T* GetDocMode()
        {
            return (T*)m_pDocMode.get();
        }
    };
}
