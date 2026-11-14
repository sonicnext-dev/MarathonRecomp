#include "guest_thread.h"
#include <stdafx.h>
#include <api/Marathon.h>
#include <fmt/format.h>
#include <kernel/memory.h>
#include <kernel/heap.h>
#include <kernel/function.h>
#include <os/logger.h>
#include "ppc_context.h"

constexpr size_t PCR_SIZE = 0xAB0;
constexpr size_t TLS_SIZE = 0x100;
constexpr size_t TEB_SIZE = 0x2E0;
constexpr size_t STACK_SIZE = 0x80000;
constexpr size_t TOTAL_SIZE = PCR_SIZE + TLS_SIZE + TEB_SIZE + STACK_SIZE;

constexpr size_t TEB_OFFSET = PCR_SIZE + TLS_SIZE;

GuestThreadContext::GuestThreadContext(uint32_t cpuNumber)
{
    assert(thread == nullptr);

    // Guest may not be up yet, allocate directly
    thread = (uint8_t*)g_memory.Alloc(TOTAL_SIZE, 0, false);
    memset(thread, 0, TOTAL_SIZE);

    *(uint32_t*)thread = ByteSwap(g_memory.MapVirtual(thread + PCR_SIZE)); // tls pointer
    *(uint32_t*)(thread + 0x100) = ByteSwap(g_memory.MapVirtual(thread + PCR_SIZE + TLS_SIZE)); // teb pointer
    *(thread + 0x10C) = cpuNumber;

    *(uint32_t*)(thread + PCR_SIZE + 0x10) = 0xFFFFFFFF; // that one TLS entry that felt quirky
    *(uint32_t*)(thread + PCR_SIZE + TLS_SIZE + 0x14C) = ByteSwap(GuestThread::GetCurrentThreadId()); // thread id

    ppcContext.r1.u64 = g_memory.MapVirtual(thread + PCR_SIZE + TLS_SIZE + TEB_SIZE + STACK_SIZE); // stack pointer
    ppcContext.r13.u64 = g_memory.MapVirtual(thread);
    ppcContext.fpscr.loadFromHost();

    assert(GetPPCContext() == nullptr);
    SetPPCContext(ppcContext);
}

GuestThreadContext::~GuestThreadContext()
{
    g_memory.Free(thread, false);
}

static std::string GetGuestThreadName(const GuestThreadParams& params)
{
    // Sonicteam::SoX::Thread::ThreadProc
    if (params.function == 0x825866A8) {
        return static_cast<Sonicteam::SoX::Thread*>(g_memory.Translate(params.value))->m_pName.get();
    }
    return fmt::format("Guest Thread (0x{:x})", params.function);
}

static void GuestThreadFunc(void* data)
{
    auto* hThread = static_cast<GuestThreadHandle*>(data);
    hThread->suspended.wait(true);
    GuestThread::Start(hThread->params);
    // HACK(1)
    hThread->isFinished = true;
}

GuestThreadHandle::GuestThreadHandle(const GuestThreadParams& params)
    : params(params), suspended((params.flags & 0x1) != 0),
      thread(GetGuestThreadName(params), GuestThreadFunc, this)
{
}

template <typename ThreadType>
static uint32_t CalcThreadId(const ThreadType& id)
{
    if constexpr (sizeof(id) == 4)
        return *reinterpret_cast<const uint32_t*>(&id);
    else
        return XXH32(&id, sizeof(id), 0);
}

uint32_t GuestThreadHandle::GetThreadId() const
{
    return CalcThreadId(thread.GetID());
}

uint32_t GuestThreadHandle::Wait(uint32_t timeout)
{
    if (timeout == INFINITE || isFinished.load()) // HACK(1): isFinished
    {
        thread.Join();
        return STATUS_WAIT_0;
    }
    else if (timeout == 0)
    {
        if (thread.IsAlive())
            return STATUS_TIMEOUT;

        return STATUS_WAIT_0;
    }
    else
    {
        auto start = std::chrono::steady_clock::now();
        while (thread.IsAlive())
        {
            auto elapsed = std::chrono::steady_clock::now() - start;
            if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() >= timeout)
                return STATUS_TIMEOUT;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return STATUS_WAIT_0;
    }
}

uint32_t GuestThread::Start(const GuestThreadParams& params)
{
    const auto procMask = (uint8_t)(params.flags >> 24);
    const auto cpuNumber = procMask == 0 ? 0 : 7 - std::countl_zero(procMask);

    GuestThreadContext ctx(cpuNumber);
    ctx.ppcContext.r3.u64 = params.value;

    g_memory.FindFunction(params.function)(ctx.ppcContext, g_memory.base);

    return ctx.ppcContext.r3.u32;
}

GuestThreadHandle* GuestThread::Start(const GuestThreadParams& params, uint32_t* threadId)
{
    auto hThread = CreateKernelObject<GuestThreadHandle>(params);

    if (threadId != nullptr)
    {
        *threadId = hThread->GetThreadId();
    }

    return hThread;
}

uint32_t GuestThread::GetCurrentThreadId()
{
    return CalcThreadId(HostThread::GetCurrentID());
}

void GuestThread::SetLastError(uint32_t error)
{
    auto* thread = (char*)g_memory.Translate(GetPPCContext()->r13.u32);
    if (*(uint32_t*)(thread + 0x150))
    {
        // Program doesn't want errors
        return;
    }

    // TEB + 0x160 : Win32LastError
    *(uint32_t*)(thread + TEB_OFFSET + 0x160) = ByteSwap(error);
}

uint32_t XSetThreadProcessor(GuestThreadHandle* hThread, uint32_t dwIdealProcessor)
{
    return 0;
}

GUEST_FUNCTION_HOOK(sub_82537F80, XSetThreadProcessor);
