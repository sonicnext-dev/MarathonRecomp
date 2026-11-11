#pragma once

#include <functional>
#include <queue>
#include <unordered_set>

template <typename Key, typename Value>
class UniqueQueue
{
private:
    std::unordered_set<Key> m_UniqueKeys{};
    std::queue<std::pair<Key, Value>> m_Queue{};

public:
    bool Empty() const
    {
        return m_Queue.empty();
    }

    size_t Size() const
    {
        return m_Queue.size();
    }

    bool Push(const Key& in_rKey, const Value& in_rValue)
    {
        if (m_UniqueKeys.insert(in_rKey).second)
        {
            m_Queue.push({ in_rKey, std::move(in_rValue) });
            return true;
        }

        return false;
    }

    Value& Front()
    {
        return m_Queue.front().second;
    }

    bool Pop()
    {
        if (m_Queue.empty())
            return false;

        m_UniqueKeys.erase(m_Queue.front().first);
        m_Queue.pop();

        return true;
    }
};
