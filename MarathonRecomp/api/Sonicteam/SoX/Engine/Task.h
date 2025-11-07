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
        char m_Flag1;
        MARATHON_INSERT_PADDING(3);
        be<uint32_t> m_Timestamp;
        xpointer<Task> m_pPrevSibling;
        xpointer<Task> m_pNextSibling;
        xpointer<Task> m_pDependency;
        xpointer<Task> m_pDependencies;
        xpointer<Doc> m_pDoc;
        LinkNode<Task> m_lnTask;

        class Iterator
        {
        public:
            Iterator(Task* current = nullptr) : m_current(current) {}

            Task& operator*() const { return *m_current; }
            Task* operator->() const { return m_current; }

            Iterator& operator++()
            {
                if (m_current)
                    m_current = m_current->m_pPrevSibling.get();
                return *this;
            }

            bool operator!=(const Iterator& other) const
            {
                return m_current != other.m_current;
            }

        private:
            Task* m_current;
        };

        Iterator begin() { return Iterator(this); }
        Iterator end() { return Iterator(nullptr); }

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

        template <typename T = Engine::Doc>
        T* GetDoc()
        {
            return (T*)m_pDoc.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(Task, m_Flag1, 0x24);
    MARATHON_ASSERT_OFFSETOF(Task, m_Timestamp, 0x28);
    MARATHON_ASSERT_OFFSETOF(Task, m_pPrevSibling, 0x2C);
    MARATHON_ASSERT_OFFSETOF(Task, m_pNextSibling, 0x30);
    MARATHON_ASSERT_OFFSETOF(Task, m_pDependency, 0x34);
    MARATHON_ASSERT_OFFSETOF(Task, m_pDependencies, 0x38);
    MARATHON_ASSERT_OFFSETOF(Task, m_pDoc, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Task, m_lnTask, 0x40);
    MARATHON_ASSERT_SIZEOF(Task, 0x4C);
}
