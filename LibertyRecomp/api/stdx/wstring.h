#pragma once

#include <kernel/heap.h>
#include <kernel/function.h>

namespace stdx
{
    class wstring
    {
    private:
        union _Bxty
        {
            _Bxty() {}

            uint16_t _buffer[8];
            xpointer<const uint16_t> _str;
        };

        be<uint32_t> _Myproxy;
        _Bxty _bx;
        be<uint32_t> _Mysize;
        be<uint32_t> _Myres;

        bool is_short() const
        {
            return _Mysize < 8;
        }

    public:
        const uint16_t* c_str() const
        {
            return is_short() ? (const uint16_t*)&_bx._buffer : (const uint16_t*)_bx._str.get();
        }

        size_t size() const
        {
            return _Mysize.get();
        }

        size_t capacity() const
        {
            return _Myres.get();
        }

        wstring()
        {
            _Myres = 0xF;
            _Mysize = 0;
            _bx._buffer[0] = '\0';
            _bx._buffer[1] = '\0';
        }

        wstring(xpointer<const uint16_t> str) : wstring(str.get()) {}

        wstring(const uint16_t* str)
        {
            _Myres = 0xF;
            _Mysize = 0;
            _bx._buffer[0] = '\0';
            _bx._buffer[1] = '\0';

            auto len = strlenU16(str);

            if (len <= 0xF)
            {
                memcpy((void*)&_bx._buffer, str, len + 2);
                _Mysize = (uint32_t)(len);
            }
            else
            {
                if (is_short() || capacity() < len + 2)
                {
                    auto new_buf = g_userHeap.Alloc<uint16_t>(len + 2);
                    memset((void*)(new_buf), 0, len + 2);
                    memcpy((void*)(new_buf), (const void*)(str), len + 2);
                    _bx._str = new_buf;
                    _Myres = len + 2;
                }
                else
                {
                    memcpy((void*)_bx._str.get(), (void*)str, len + 2);
                }

                _Mysize = len;
            }
        }

        ~wstring()
        {
            if (!is_short())
                g_userHeap.Free((void*)_bx._str.get());

            _Myres = 0xF;
            _Mysize = 0;
            _bx._buffer[0] = '\0';
            _bx._buffer[1] = '\0';
        }

        bool operator==(const uint16_t* str) const
        {
            return strcmpU16(c_str(), str, false, true);
        }

        bool operator==(xpointer<const uint16_t> str) const
        {
            return strcmpU16(c_str(), str);
        }
    };
};
