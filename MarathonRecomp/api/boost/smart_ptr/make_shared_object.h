#pragma once

#include "boost/smart_ptr/shared_ptr.h"

namespace boost
{
    template<typename T, typename... TArgs>
    shared_ptr<T> make_shared(TArgs&&... args)
    {
        return shared_ptr<T>(new T(std::forward<TArgs>(args)...));
    }

    template<typename T>
    shared_ptr<T> make_shared(T* pointer,uint32_t vftable)
    {
        shared_ptr<T> pointer_holder;

        auto* counted = (detail::sp_counted_impl_p<T>*)g_userHeap.Alloc(sizeof(detail::sp_counted_impl_p<T>));

        new (counted) detail::sp_counted_impl_p<T>(pointer);

        pointer_holder.px = pointer;
        pointer_holder.pn = (detail::sp_counted_base*)counted;
        pointer_holder.pn->vftable_.ptr = vftable;

        return pointer_holder;
    }
}
