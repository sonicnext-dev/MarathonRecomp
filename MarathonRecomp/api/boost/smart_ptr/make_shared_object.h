#pragma once

#include "boost/smart_ptr/shared_ptr.h"

namespace boost
{
    template<typename T>
    shared_ptr<T> make_shared(T* p, uint32_t vftable)
    {
        shared_ptr<T> sp;

        auto* counted = (detail::sp_counted_impl_p<T>*)g_userHeap.Alloc(sizeof(detail::sp_counted_impl_p<T>));

        new (counted) detail::sp_counted_impl_p<T>(p);

        sp.px = p;
        sp.pn = (detail::sp_counted_base*)counted;
        sp.pn->vftable_.ptr = vftable;

        return sp;
    }

    template<typename T>
    shared_ptr<T> make_shared(T* pointer, uint32_t vftable)
    {
        boost::shared_ptr<T> pointer_holder;

        auto* counted = (boost::detail::sp_counted_impl_p<T>*)g_userHeap.Alloc(sizeof(boost::detail::sp_counted_impl_p<T>));

        new (counted) boost::detail::sp_counted_impl_p<T>(pointer);

        pointer_holder.px = pointer;
        pointer_holder.pn = (boost::detail::sp_counted_base*)counted;
        pointer_holder.pn->vftable_.ptr = vftable;

        return pointer_holder;
    }
}
