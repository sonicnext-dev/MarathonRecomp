#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/Engine/RenderScheduler.h>
#include <Sonicteam/RootGTask.h>

namespace Sonicteam::SoX::Engine
{
    class Doc
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpUpdate;
            MARATHON_INSERT_PADDING(0x04);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(4);
        xpointer<DocMode> m_pDocMode;
        xpointer<Task> m_pRootTask;
        xpointer<RootGTask> m_pRootGTask;
        MARATHON_INSERT_PADDING(8);
        xpointer<Task> m_pDocModeExecutor;
        xpointer<RenderScheduler> m_pRenderScheduler;
        XRTL_CRITICAL_SECTION m_CriticalSection1;
        XRTL_CRITICAL_SECTION m_CriticalSection2;

        template <typename T = DocMode>
        T* GetDocMode(const char* name = nullptr)
        {
            if (!m_pDocMode)
                return nullptr;

            if (name && strcmp(m_pDocMode->GetName(), name) != 0)
                return nullptr;

            return static_cast<T*>(m_pDocMode.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(Doc, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pDocMode, 0x08);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pRootTask, 0x0C);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pRootGTask, 0x10);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pDocModeExecutor, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Doc, m_pRenderScheduler, 0x20);
    MARATHON_ASSERT_OFFSETOF(Doc, m_CriticalSection1, 0x24);
    MARATHON_ASSERT_OFFSETOF(Doc, m_CriticalSection2, 0x40);
    MARATHON_ASSERT_SIZEOF(Doc, 0x5C);
}
