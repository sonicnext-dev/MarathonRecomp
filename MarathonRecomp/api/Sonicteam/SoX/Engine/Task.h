#pragma once

#include <functional>

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
        uint8_t m_Flags;
        MARATHON_INSERT_PADDING(3);
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

        template <typename T = Doc>
        T* GetDoc()
        {
            return (T*)m_pDoc.get();
        }

        void WalkSiblings(std::function<bool(Task*)> in_walker, bool in_walkDependencies = true)
        {
            for (auto& rTask : *this)
            {
                if (!in_walker(&rTask))
                    break;

                if (in_walkDependencies && rTask.m_pDependencies)
                    rTask.m_pDependencies->WalkSiblings(in_walker);
            }
        }

        class iterator
        {
        private:
            Task* m_pCurrent{};

        public:
            iterator(Task* in_pCurrent = nullptr) : m_pCurrent(in_pCurrent) {}

            Task& operator*() const
            {
                return *m_pCurrent;
            }

            Task* operator->() const
            {
                return m_pCurrent;
            }

            iterator& operator++()
            {
                if (m_pCurrent)
                    m_pCurrent = m_pCurrent->m_pPrevSibling.get();

                return *this;
            }

            bool operator!=(const iterator& in_rOther) const
            {
                return m_pCurrent != in_rOther.m_pCurrent;
            }
        };

        iterator begin()
        {
            return iterator(this);
        }

        iterator end()
        {
            return iterator(nullptr);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Task, m_Flags, 0x24);
    MARATHON_ASSERT_OFFSETOF(Task, m_Timestamp, 0x28);
    MARATHON_ASSERT_OFFSETOF(Task, m_pPrevSibling, 0x2C);
    MARATHON_ASSERT_OFFSETOF(Task, m_pNextSibling, 0x30);
    MARATHON_ASSERT_OFFSETOF(Task, m_pDependency, 0x34);
    MARATHON_ASSERT_OFFSETOF(Task, m_pDependencies, 0x38);
    MARATHON_ASSERT_OFFSETOF(Task, m_pDoc, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Task, m_lnTask, 0x40);
    MARATHON_ASSERT_SIZEOF(Task, 0x4C);
}
