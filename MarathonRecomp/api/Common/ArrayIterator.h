#pragma once

template <typename T>
class ArrayIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

private:
    pointer m_pCurrent{};

public:
    explicit ArrayIterator(pointer in_pStart) : m_pCurrent(in_pStart) {}

    reference operator*() const
    {
        return *m_pCurrent;
    }

    pointer operator->()
    {
        return m_pCurrent;
    }

    ArrayIterator& operator++()
    {
        m_pCurrent++;
        return *this;
    }

    ArrayIterator operator++(int)
    {
        const auto _ = *this;
        m_pCurrent++;
        return _;
    }

    friend bool operator==(const ArrayIterator& in_rLeft, const ArrayIterator& in_rRight)
    {
        return in_rLeft.m_pCurrent == in_rRight.m_pCurrent;
    }

    friend bool operator!=(const ArrayIterator& in_rLeft, const ArrayIterator& in_rRight)
    {
        return in_rLeft.m_pCurrent != in_rRight.m_pCurrent;
    }
};
