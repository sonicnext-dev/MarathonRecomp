#pragma once

#include <message.h>

class IMessageReceiver
{
protected:
    virtual ~IMessageReceiver() = default;

    virtual bool OnBroadcastMessage(IMessage& io_rMessage) = 0;
    virtual bool OnProcessMessage(IMessage& io_rMessage) = 0;

public:
    bool BroadcastMessage(IMessage& io_rMessage)
    {
        return OnBroadcastMessage(io_rMessage);
    }

    template <typename T, typename... TArgs>
    bool BroadcastMessage(TArgs&&... in_args)
    {
        T msg(std::forward<TArgs>(in_args)...);
        return OnBroadcastMessage(msg);
    }

    bool ProcessMessage(IMessage& io_rMessage)
    {
        return OnProcessMessage(io_rMessage);
    }

    template <typename T, typename... TArgs>
    bool ProcessMessage(TArgs&&... in_args)
    {
        T msg(std::forward<TArgs>(in_args)...);
        return OnProcessMessage(msg);
    }
};
