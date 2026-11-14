#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace boost
{
    namespace detail
    {
        class weak_count
        {
        protected:
            xpointer<sp_counted_base> pi_;
        };
    }

    template <typename T>
    class weak_ptr
    {
    public:
        xpointer<T> px;
        xpointer<boost::detail::weak_count> pn;
    };
}
