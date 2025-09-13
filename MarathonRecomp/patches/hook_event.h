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

template <typename T>
class IContextHookEvent
{
public:
    virtual ~IContextHookEvent() = default;
    virtual void Prefix(T* pThis) = 0;
    virtual void Update(T* pThis, float deltaTime) = 0;
    virtual void Postfix(T* pThis) = 0;
};

template <typename T>
class ContextHookEvent : public IContextHookEvent<T>
{
public:
    void Prefix(T* pThis) override {}
    void Update(T* pThis, float deltaTime) override {}
    void Postfix(T* pThis) override {}
};
