#pragma once

namespace stdx
{
    template <typename T>
    class deque
    {
    private:
        be<uint32_t> _Myproxy;
        xpointer<xpointer<T>> _Map;
        be<uint32_t> _Mapsize;
        be<uint32_t> _Myoff;
        be<uint32_t> _Mysize;
    };
}
