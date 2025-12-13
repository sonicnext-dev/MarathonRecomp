#pragma once

template<typename T>
struct FreeList
{
    std::vector<T> items;
    std::vector<size_t> freed{};

    void Free(T& item)
    {
        std::destroy_at(&item);
        freed.push_back(&item - items.data());
    }

    void Free(size_t index)
    {
        std::destroy_at(&items[index]);
        freed.push_back(index);
    }

    size_t Alloc()
    {
        if (freed.size())
        {
            auto idx = freed[freed.size() - 1];
            freed.pop_back();

            std::construct_at(&items[idx]);
            return idx;
        }

        items.emplace_back();
        return items.size() - 1;
    }

    T& operator[](size_t idx)
    {
        return items[idx];
    }
};