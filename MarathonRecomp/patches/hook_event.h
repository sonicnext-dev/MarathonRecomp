#pragma once

class IHookEvent
{
public:
    virtual ~IHookEvent() = default;
    virtual void Prefix();
    virtual void Update(float deltaTime);
    virtual void Postfix();
};

class HookEvent : public IHookEvent
{
public:
    void Prefix() override {}
    void Update(float deltaTime) override {}
    void Postfix() override {}
};
