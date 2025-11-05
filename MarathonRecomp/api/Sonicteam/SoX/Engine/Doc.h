#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <Sonicteam/SoX/Engine/DocModeExecutor.h>
#include <Sonicteam/SoX/Engine/RenderScheduler.h>
#include <Sonicteam/SoX/Engine/RootGTask.h>
#include <Sonicteam/SoX/Engine/RootTask.h>

namespace Sonicteam::SoX::Engine
{
    class Doc
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(4);
        xpointer<DocMode> m_pDocMode;
        xpointer<RootTask> m_pRootTask;
        xpointer<RootGTask> m_pRootGTask;
        xpointer<ApplicationXenon> m_pDocCurrentApplication;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<DocModeExecutor> m_pDocModeExecutor;
        xpointer<RenderScheduler> m_pRenderScheduler;
        MARATHON_INSERT_PADDING(0x38);

        template <typename T = DocMode>
        inline T* GetDocMode()
        {
            return (T*)m_pDocMode.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(Doc, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pDocMode, 0x08);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pRootTask, 0x0C);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pRootGTask, 0x10);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pDocModeExecutor, 0x1C);
    MARATHON_ASSERT_SIZEOF(Doc, 0x5C);
}
