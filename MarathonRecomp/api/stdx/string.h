#pragma once

#include <kernel/heap.h>
#include <kernel/function.h>


namespace stdx
{
    class string
    {
    private:
        union _Bxty
        {
            _Bxty() {}

            char _buffer[0x10];
            xpointer<const char> _str;
        };

        be<uint32_t> _Myproxy;
        _Bxty _bx;
        be<uint32_t> _Mysize;
        be<uint32_t> _Myres;

        bool is_short() const
        {
            return _Mysize <= 0xF;
        }

    public:
        const char* c_str() const
        {
            return is_short() ? (const char*)&_bx._buffer : (const char*)_bx._str.get();
        }

        size_t size() const
        {
            return _Mysize;
        }

        size_t capacity() const
        {
            return _Myres;
        }

        string()
        {
            _Myres = 0xF;
            _Mysize = 0;
            _bx._buffer[0] = '\0';
        }

        string(xpointer<const char> str) : string(str.get()) {}

        string(const char* str)
        {
            _Myres = 0xF;
            _Mysize = 0;
            _bx._buffer[0] = '\0';
            from_cstr(str);
        }

        ~string()
        {
            cleanup();
            _Myres = 0xF;
            _Mysize = 0;
            _bx._buffer[0] = '\0';
        }


        void cleanup()
        {
            if (!is_short()) {
                g_userHeap.Free((void*)_bx._str.get());
            }
        }

        void from_cstr(const char* str)
        {
            auto len = strlen(str);
            if (len <= 0xF)
            {
                memcpy((void*)&_bx._buffer, str, len + 1);
                _Mysize = (uint32_t)(len);
            }
            else
            {
                if (is_short() || capacity() < len + 1)
                {
                    cleanup();
                    char* new_buf = g_userHeap.Alloc<char>(len + 1);
                    memset((void*)(new_buf), 0, len + 1);
                    memcpy((void*)(new_buf), (const void*)(str), len + 1);
                    _bx._str = new_buf;
                    _Myres = len + 1;
                }
                else
                {
                    memcpy((void*)_bx._str.get(), (void*)str, len + 1);
                }

                _Mysize = len;
            }
        }

        string& operator=(const char* str)
        {
            from_cstr(str);
            return *this;
        }

        bool operator==(const char* str) const
        {
            return strcmp(c_str(), str) == 0;
        }
    };
};
