#pragma once

class IHookEvent
{
public:
    virtual ~IHookEvent() = default;
    virtual void Prefix() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Postfix() = 0;
};

class HookEvent : public IHookEvent
{
public:
    void Prefix() override {}
    void Update(float deltaTime) override {}
    void Postfix() override {}
};
