#pragma once

#include <type_traits>
#include <boost/move/unique_ptr.hpp>
#include <kernel/heap.h>
#include <kernel/memory.h>

namespace boost
{
    template <typename T>
    requires (!std::is_array_v<T>)
    unique_ptr<T> make_unique_noinit()
    {
        unique_ptr<T> result{};

        result.p = reinterpret_cast<T*>(g_userHeap.AllocPhysical(sizeof(T), alignof(T)));

        return result;
    }

    template <typename T>
    requires std::is_array_v<T>
    unique_ptr<std::remove_pointer_t<std::decay_t<T>>> make_unique_noinit(size_t size)
    {
        using TUnderlyingType = std::remove_pointer_t<std::decay_t<T>>;

        unique_ptr<TUnderlyingType> result{};

        result.p = reinterpret_cast<TUnderlyingType*>(g_userHeap.AllocPhysical(sizeof(TUnderlyingType) * size, alignof(TUnderlyingType)));

        return result;
    }

    template <typename T>
    requires (!std::is_array_v<T>)
    unique_ptr<T> make_unique()
    {
        auto result = make_unique_noinit<T>();

        *result.p = T();

        return result;
    }

    template <typename T>
    requires (!std::is_array_v<T>)
    unique_ptr<T> make_unique(T&& value)
    {
        auto result = make_unique<T>();

        *result.p = value;

        return result;
    }

    template <typename T>
    requires std::is_array_v<T>
    unique_ptr<std::remove_pointer_t<std::decay_t<T>>> make_unique(size_t size)
    {
        using TUnderlyingType = std::remove_pointer_t<std::decay_t<T>>;

        auto result = make_unique_noinit<T>(size);

        for (size_t i = 0; i < size; i++)
            result.p.get()[i] = TUnderlyingType();

        return result;
    }

    template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
    unique_ptr<T> make_unique(Args&&... args)
    {
        auto result = make_unique<T>();

        new (result.get()) T(std::forward<Args>(args)...);

        return result;
    }
}
