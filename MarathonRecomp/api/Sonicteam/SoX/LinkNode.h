#pragma once

namespace Sonicteam::SoX
{
    template <typename T>
    class ILinkNode
    {
    public:
        xpointer<T> m_pPrev;
        xpointer<T> m_pNext;

        void InsertFast(T* nextNode)
        {
            nextNode->m_pPrev = static_cast<T*>(this);
            nextNode->m_pNext = this->m_pNext;

            this->m_pNext->m_pPrev = nextNode;
            this->m_pNext = nextNode;
        }

        void Insert(T* nextNode)
        {
            if (!nextNode) return;

            nextNode->m_pPrev = static_cast<T*>(this);
            nextNode->m_pNext = this->m_pNext;

            if (this->m_pNext)
                this->m_pNext->m_pPrev = nextNode;

            this->m_pNext = nextNode;
        }

        void ResetList()
        {
            T* current = this->m_pNext.get();

            while (current && current != this) 
            {
                T* next = current->m_pNext.get();
                current->m_pPrev = nullptr;
                current->m_pNext = nullptr;
                current = next;
            }

            if (this->m_pPrev) this->m_pPrev->m_pNext = this->m_pNext;
            if (this->m_pNext) this->m_pNext->m_pPrev = this->m_pPrev;
        }

        void Reset()
        {
            if (this->m_pPrev) this->m_pPrev->m_pNext = this->m_pNext;
            if (this->m_pNext) this->m_pNext->m_pPrev = this->m_pPrev;
            this->m_pPrev = 0;
            this->m_pNext = 0;
        }

        void ResetFast()
        {
            this->m_pPrev->m_pNext = this->m_pNext;
            this->m_pNext->m_pPrev = this->m_pPrev;
            this->m_pPrev = 0;
            this->m_pNext = 0;
        }

        class iterator
        {
            T* m_pCurrent;
            T* m_pStart;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            explicit iterator(T* start = nullptr)
                : m_pCurrent(start ? start->m_pNext.get() : nullptr)
                , m_pStart(start)
            {
                if (m_pStart && m_pStart->m_pNext.get() == m_pStart)
                {
                    m_pCurrent = nullptr;
                }
                else if (m_pStart)
                {
                    m_pCurrent = m_pStart->m_pNext.get();
                }
            }

            reference operator*() const
            {
                return *m_pCurrent;
            }

            pointer operator->() const
            {
                return m_pCurrent;
            }

            iterator& operator++()
            {
                if (m_pCurrent && m_pCurrent->m_pNext)
                {
                    m_pCurrent = m_pCurrent->m_pNext.get();

                    // Stop if we reach the start node again.
                    if (m_pCurrent == m_pStart)
                        m_pCurrent = nullptr;
                }
                else
                {
                    m_pCurrent = nullptr;
                }

                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const
            {
                return m_pCurrent == other.m_pCurrent;
            }

            bool operator!=(const iterator& other) const
            {
                return m_pCurrent != other.m_pCurrent;
            }
        };

        class const_iterator
        {
            const T* m_pCurrent;
            const T* m_pStart;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = const T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T*;
            using reference = const T&;

            explicit const_iterator(const T* start = nullptr)
                : m_pCurrent(start ? start->m_pNext.get() : nullptr)
                , m_pStart(start)
            {
                if (m_pStart && m_pStart->m_pNext.get() == m_pStart)
                {
                    m_pCurrent = nullptr;
                }
                else if (m_pStart)
                {
                    m_pCurrent = m_pStart->m_pNext.get();
                }
            }

            const_iterator(const iterator& it)
                : m_pCurrent(it.m_pCurrent)
                , m_pStart(it.m_pStart)
            {
            }

            reference operator*() const
            {
                return *m_pCurrent;
            }

            pointer operator->() const
            {
                return m_pCurrent;
            }

            const_iterator& operator++()
            {
                if (m_pCurrent && m_pCurrent->m_pNext)
                {
                    m_pCurrent = m_pCurrent->m_pNext.get();

                    // Stop if we reach the start node again.
                    if (m_pCurrent == m_pStart)
                        m_pCurrent = nullptr;
                }
                else
                {
                    m_pCurrent = nullptr;
                }

                return *this;
            }

            const_iterator operator++(int)
            {
                const_iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const const_iterator& other) const
            {
                return m_pCurrent == other.m_pCurrent;
            }

            bool operator!=(const const_iterator& other) const
            {
                return m_pCurrent != other.m_pCurrent;
            }
        };

        iterator begin()
        {
            return iterator(static_cast<T*>(this));
        }

        iterator end()
        {
            return iterator(nullptr);
        }

        const_iterator begin() const
        {
            return const_iterator(static_cast<const T*>(this));
        }

        const_iterator end() const
        {
            return const_iterator(nullptr);
        }

        const_iterator cbegin() const
        {
            return begin();
        }

        const_iterator cend() const
        {
            return end();
        }
    };

    template <typename T>
    class LinkNode : public ILinkNode<LinkNode<T>>
    {
    public:
        xpointer<T> m_pThis;

        void ForEach(std::function<void(T*)> callback)
        {
            for (auto& node : *this) {
                if (node.m_pThis) {
                    callback(node.m_pThis.get());
                }
            }
        }

        typedef LinkNode<T> IType;
        typedef ILinkNode<LinkNode<T>> INodeType;
        typedef T pThisType;
    };

    template <typename T>
    using LinkedList = LinkNode<T>;

    template <typename T, typename Y = T>
    class LinkRef
    {
    public:
        xpointer<Y> m_pElement;
        LinkNode<LinkRef<T,Y>> m_lnElement;

        typedef LinkNode<LinkRef<T>> lnElementType;
        typedef T pElementType;
    };

}
