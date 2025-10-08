#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Physics/Entity.h>

namespace Sonicteam::SoX::Physics
{
    class EntityRef : public LinkRef<EntityRef,Entity>
    {
    public:
        ~EntityRef()
        {
            ClearRef();
            m_lnElement.Reset();
        }

        EntityRef() noexcept
        {
            InitializeEmpty();
        }

        EntityRef(const EntityRef& other)
        {
            InitializeEmpty();
            CopyFrom(other);
        }

        EntityRef& operator=(const EntityRef& other)
        {
            if (this != &other)
            {
                ClearRef();
                InitializeEmpty();
                CopyFrom(other);
            }
            return *this;
        }

        EntityRef(EntityRef&& other) noexcept
        {
            InitializeEmpty();
            MoveFrom(std::move(other));
        }

        EntityRef& operator=(EntityRef&& other) noexcept
        {
            if (this != &other)
            {
                ClearRef();
                InitializeEmpty();
                MoveFrom(std::move(other));
            }
            return *this;
        }

        void ClearRef() noexcept
        {
            if (m_lnElement.m_pNext)
            {
                m_lnElement.ResetFast();
            }
        }

        bool sub_82595500(Entity* entityRef)
        {
            if (m_pElement && m_pElement->IsInitializedToWorld())
                return entityRef != m_pElement.get();
            else
                return entityRef != nullptr;
        }

        Entity* sub_82595020()
        {
            if (m_pElement && m_pElement->IsInitializedToWorld())
                return m_pElement.get();
            return nullptr;
        }

    private:
        void InitializeEmpty() noexcept
        {
            m_pElement = nullptr;
            m_lnElement.m_pNext = nullptr;
            m_lnElement.m_pPrev = nullptr;
            m_lnElement.m_pThis = this;
        }

        void CopyFrom(const EntityRef& other)
        {
            if (other.m_pElement && other.m_pElement->IsInitializedToWorld())
            {
                m_pElement = other.m_pElement;
                other.m_pElement->m_lnEntity.InsertFast((LinkNode<EntityRef>::IType*) & m_lnElement);
            }
        }

        void MoveFrom(EntityRef&& other) noexcept
        {
            CopyFrom(other);
            other.ClearRef();
            other.InitializeEmpty();
        }
    };

    class EntityRefWrapper
    {
    private:
        xpointer<EntityRef> m_EntityRef;
        int* m_refCount;

        void add_ref()
        {
            if (m_refCount) {
                ++(*m_refCount);
            }
        }

        void release()
        {
            if (m_refCount && --(*m_refCount) == 0) {
                if (m_EntityRef.get()) {
                    m_EntityRef->~EntityRef();
                    g_userHeap.Free(m_EntityRef.get());
                }
                delete m_refCount;
                m_EntityRef = nullptr;
                m_refCount = nullptr;
            }
        }

    public:

        EntityRefWrapper() : m_refCount(new int(1))
        {
            m_EntityRef = g_userHeap.AllocPhysical<EntityRef>();
        }

        explicit EntityRefWrapper(EntityRef* ref) : m_refCount(new int(1))
        {
            m_EntityRef = ref;
        }

        EntityRefWrapper(const EntityRefWrapper& other)
            : m_EntityRef(other.m_EntityRef), m_refCount(other.m_refCount)
        {
            add_ref();
        }

        EntityRefWrapper& operator=(const EntityRefWrapper& other)
        {
            if (this != &other) {
                release();
                m_EntityRef = other.m_EntityRef;
                m_refCount = other.m_refCount;
                add_ref();
            }
            return *this;
        }

        EntityRefWrapper(EntityRefWrapper&& other) noexcept
            : m_EntityRef(std::move(other.m_EntityRef))
            , m_refCount(other.m_refCount)
        {
            other.m_refCount = nullptr;
        }

        EntityRefWrapper& operator=(EntityRefWrapper&& other) noexcept
        {
            if (this != &other) {
                release();
                m_EntityRef = std::move(other.m_EntityRef);
                m_refCount = other.m_refCount;
                other.m_refCount = nullptr;
            }
            return *this;
        }

        ~EntityRefWrapper()
        {
            release();
        }

        // Dereference operators
        EntityRef* operator->() const { return m_EntityRef.get(); }
        EntityRef& operator*() const { return *m_EntityRef.get(); }

        // Getters
        EntityRef* get() const { return m_EntityRef.get(); }
        int use_count() const { return m_refCount ? *m_refCount : 0; }
        bool unique() const { return use_count() == 1; }

        // Reset functionality
        void reset(EntityRef* ptr = nullptr)
        {
            release();
            if (ptr) {
                m_EntityRef = ptr;
                m_refCount = new int(1);
            }
        }

        explicit operator bool() const { return m_EntityRef.get() != nullptr; }

        inline void swap(EntityRefWrapper& other) noexcept
        {
            std::swap(m_EntityRef, other.m_EntityRef);
            std::swap(m_refCount, other.m_refCount);
        }
    };
}
