#pragma once

inline uint32_t CreateMessageID()
{
    static std::atomic<uint32_t> id{};
    return id++;
}

class IMessage
{
private:
    uint32_t m_ID{};

public:
    explicit IMessage(uint32_t in_id) : m_ID(in_id) {}
    virtual ~IMessage() = default;

    uint32_t GetID() const
    {
        return m_ID;
    }

    template <typename T>
    bool Is() const
    {
        return m_ID == T::GetStaticID();
    }
};

template <typename T>
class Message : public IMessage
{
protected:
    Message() : IMessage(Message<T>::GetStaticID()) {}

public:
    static uint32_t GetStaticID()
    {
        static const uint32_t id = CreateMessageID();
        return id;
    }
};
