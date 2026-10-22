#pragma once

#include <boost/smart_ptr/shared_ptr.hpp>

namespace boost
{
    // TODO: infer vftables from MARATHON_VIRTUAL_BASE.
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
}
