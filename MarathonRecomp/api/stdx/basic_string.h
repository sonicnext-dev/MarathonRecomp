#pragma once

#include <algorithm>
#include <kernel/heap.h>
#include <kernel/function.h>

namespace stdx
{
    template <typename TChar>
    class basic_string
    {
    private:
        static constexpr size_t _ShortBufferSize = 0x10 / sizeof(TChar);
        static constexpr size_t _ShortBufferSizeTerminated = _ShortBufferSize - 1;

        union _Bxty
        {
            _Bxty() {}

            TChar _buffer[_ShortBufferSize];
            xpointer<TChar> _str;
        };

        be<uint32_t> _Myproxy;
        _Bxty _bx;
        be<uint32_t> _Mysize;
        be<uint32_t> _Myres;

        bool is_short() const
        {
            return _Mysize <= _ShortBufferSizeTerminated;
        }

        void cleanup()
        {
            if (is_short())
                return;

            g_userHeap.Free(_bx._str.get());

            _bx._str = nullptr;
            _Mysize = 0;
            _Myres = _ShortBufferSizeTerminated;
        }

        void from_cstr(std::basic_string_view<TChar> str, bool endianSwap = false)
        {
            const auto lengthInChars = str.size();
            const auto lengthInBytes = lengthInChars * sizeof(TChar);

            if (lengthInChars <= _ShortBufferSizeTerminated)
            {
                cleanup();

                if (lengthInChars > 0)
                    memcpy(reinterpret_cast<void*>(_bx._buffer), str.data(), lengthInBytes);

                _bx._buffer[lengthInChars] = 0;
            }
            else
            {
                const auto lengthInCharsTerminated = lengthInChars + 1;
                const auto lengthInBytesTerminated = lengthInCharsTerminated * sizeof(TChar);

                if (is_short() || capacity() < lengthInCharsTerminated)
                {
                    cleanup();

                    _bx._str = reinterpret_cast<TChar*>(g_userHeap.Alloc(lengthInBytesTerminated));
                    _Myres = lengthInCharsTerminated;
                }

                memcpy(_bx._str.get(), str.data(), lengthInBytes);

                _bx._str.get()[lengthInChars] = 0;
            }

            _Mysize = lengthInChars;

            if (endianSwap && sizeof(TChar) > 1)
            {
                // Endian swap Unicode strings.
                for (size_t i = 0; i < size(); i++)
                    ByteSwapInplace(const_cast<TChar*>(c_str())[i]);
            }
        }

    public:
        basic_string()
        {
            _bx._buffer[0] = 0;
            _Mysize = 0;
            _Myres = _ShortBufferSizeTerminated;
        }

        basic_string(const basic_string& other) : basic_string()
        {
            from_cstr(other.c_str());
        }

        basic_string(basic_string&& other) noexcept
        {
            _Myproxy = other._Myproxy;
            _bx = other._bx;
            _Mysize = other._Mysize;
            _Myres = other._Myres;

            other._bx._buffer[0] = 0;
            other._Mysize = 0;
            other._Myres = _ShortBufferSizeTerminated;
        }

        basic_string(const TChar* str, bool endianSwap = true) : basic_string()
        {
            if (!str)
            {
                cleanup();
                return;
            }

            from_cstr(str, endianSwap);
        }

        basic_string(xpointer<const TChar> str) : basic_string(str.get(), false) {}

        ~basic_string()
        {
            cleanup();

            _bx._buffer[0] = 0;
            _Mysize = 0;
            _Myres = _ShortBufferSizeTerminated;
        }

        const TChar* c_str() const
        {
            return is_short()
                ? reinterpret_cast<const TChar*>(&_bx._buffer)
                : reinterpret_cast<const TChar*>(_bx._str.get());
        }

        size_t size() const
        {
            return _Mysize.get();
        }

        size_t capacity() const
        {
            return _Myres.get();
        }

        bool empty() const
        {
            return !size();
        }

        basic_string& operator=(const basic_string& other)
        {
            if (this != &other)
                from_cstr(other.c_str());

            return *this;
        }

        basic_string& operator=(basic_string&& other) noexcept
        {
            if (this != &other)
            {
                cleanup();

                _Myproxy = other._Myproxy;
                _bx = other._bx;
                _Mysize = other._Mysize;
                _Myres = other._Myres;

                other._bx._buffer[0] = 0;
                other._Mysize = 0;
                other._Myres = _ShortBufferSizeTerminated;
            }

            return *this;
        }

        basic_string<TChar>& operator=(const TChar* str)
        {
            from_cstr(str, true);

            return *this;
        }

        basic_string<TChar>& operator=(xpointer<const TChar> str)
        {
            from_cstr(str, false);

            return *this;
        }

        bool operator==(const TChar* str) const
        {
            return memcmp(c_str(), str, size()) == 0;
        }

        bool operator<(const TChar* str) const
        {
            return memcmp(c_str(), str, size()) < 0;
        }

        const TChar* begin() const
        {
            return c_str();
        }

        const TChar* end() const
        {
            return c_str() + size();
        }

        bool operator<(const basic_string& other) const
        {
            return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
        }

        bool operator>(const basic_string& other) const
        {
            return other < *this;
        }

        bool operator<=(const basic_string& other) const
        {
            return !(other < *this);
        }

        bool operator>=(const basic_string& other) const
        {
            return !(*this < other);
        }

        bool operator==(const basic_string& other) const
        {
            return size() == other.size() && std::equal(begin(), end(), other.begin());
        }

        bool operator!=(const basic_string& other) const
        {
            return !(*this == other);
        }
    };
};
