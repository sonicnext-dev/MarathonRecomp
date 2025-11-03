#pragma once

namespace Sonicteam::SoX
{
    template <typename T>
    class ILinkNode
    {
    public:
        xpointer<T> m_pPrev;
        xpointer<T> m_pNext;

        // Iterator class
        class iterator
        {
        private:
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
            }

            reference operator*() const
            {
                return *m_pCurrent;
            }

            pointer operator->() const
            {
                return m_pCurrent;
            }

            // Prefix increment - traverse to next node
            iterator& operator++()
            {
                if (m_pCurrent && m_pCurrent->m_pNext)
                {
                    m_pCurrent = m_pCurrent->m_pNext.get();
                    // Stop if we reach the start node again
                    if (m_pCurrent == m_pStart)
                    {
                        m_pCurrent = nullptr;
                    }
                }
                else
                {
                    m_pCurrent = nullptr;
                }
                return *this;
            }

            // Postfix increment
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

        // Const iterator
        class const_iterator
        {
        private:
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
                    // Stop if we reach the start node again
                    if (m_pCurrent == m_pStart)
                    {
                        m_pCurrent = nullptr;
                    }
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

        // Iteration support
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
    };

    template <typename T>
    using LinkedList = LinkNode<T>;

    template <typename T>
    class LinkRef
    {
    public:
        xpointer<T> m_pElement;
        LinkNode<LinkRef> m_lnElement;
    };
}
