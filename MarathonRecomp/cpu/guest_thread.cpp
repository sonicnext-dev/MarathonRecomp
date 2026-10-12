#include "guest_thread.h"
#include <cstdio>
#include <stdafx.h>
#include <api/Marathon.h>
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

    thread = (uint8_t*)g_userHeap.Alloc(TOTAL_SIZE);
    // printf("TOTAL_SIZE: %x %x %d\n", thread, TOTAL_SIZE, TOTAL_SIZE);
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
    g_userHeap.Free(thread);
}

#ifdef USE_PTHREAD
static size_t GetStackSize()
{
    // Cache as this should not change.
    static size_t stackSize = 0;
    if (stackSize == 0)
    {
        // 8 MiB is a typical default.
        constexpr auto defaultSize = 8 * 1024 * 1024;
        struct rlimit lim;
        const auto ret = getrlimit(RLIMIT_STACK, &lim);
        if (ret == 0 && lim.rlim_cur < defaultSize)
        {
            // Use what the system allows.
            stackSize = lim.rlim_cur;
        }
        else
        {
            stackSize = defaultSize;
        }
    }
    return stackSize;
}

static void* GuestThreadFunc(void* arg)
{
    GuestThreadHandle* hThread = (GuestThreadHandle*)arg;
#else
static void* GuestThreadFunc(GuestThreadHandle* hThread)
{
#endif
    hThread->suspended.wait(true);
    GuestThread::Start(hThread->params);
    // HACK(1)
    hThread->isFinished = true;
    return nullptr;
}

GuestThreadHandle::GuestThreadHandle(const GuestThreadParams& params)
    : params(params), suspended((params.flags & 0x1) != 0)
#ifdef USE_PTHREAD
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, GetStackSize());
    const auto ret = pthread_create(&thread, &attr, GuestThreadFunc, this);
    if (ret != 0) {
        fprintf(stderr, "pthread_create failed with error code 0x%X.\n", ret);
        return;
    }
}
#else
, thread(GuestThreadFunc, this)
{
}
#endif

GuestThreadHandle::~GuestThreadHandle()
{
#ifdef USE_PTHREAD
    pthread_join(thread, nullptr);
#else
    if (thread.joinable())
        thread.join();
#endif
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
#ifdef USE_PTHREAD
    return CalcThreadId(thread);
#else
    return CalcThreadId(thread.get_id());
#endif
}

uint32_t GuestThreadHandle::Wait(uint32_t timeout)
{
    if (timeout == INFINITE || isFinished.load()) // HACK(1): isFinished
    {
#ifdef USE_PTHREAD
        pthread_join(thread, nullptr);
#else
        if (thread.joinable())
            thread.join();
#endif

        return STATUS_WAIT_0;
    }
    else if (timeout == 0)
    {
#ifndef USE_PTHREAD
        if (thread.joinable())
            return STATUS_TIMEOUT;
#endif

        return STATUS_WAIT_0;
    }
    else
    {
#ifdef USE_PTHREAD
        pthread_join(thread, nullptr);
#else
        auto start = std::chrono::steady_clock::now();
        while (thread.joinable())
        {
            auto elapsed = std::chrono::steady_clock::now() - start;
            if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() >= timeout)
                return STATUS_TIMEOUT;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
#endif

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
#ifdef USE_PTHREAD
    return CalcThreadId(pthread_self());
#else
    return CalcThreadId(std::this_thread::get_id());
#endif
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

void GuestThread::SetThreadName(const char* name)
{
#if defined(_WIN32)

    //
    // Reference: https://learn.microsoft.com/visualstudio/debugger/how-to-set-a-thread-name-in-native-code
    //

#if defined(NTDDI_VERSION) && (NTDDI_VERSION < NTDDI_WIN10_RS1)

    //
    // Legacy thread naming for Windows 10 1511 or earlier.
    //

#pragma pack(push, 8)
    struct THREADNAME_INFO
    {
        DWORD dwType{ 0x1000 };
        LPCSTR szName{};
        DWORD dwThreadID{};
        DWORD dwFlags{};
    };
#pragma pack(pop)

    THREADNAME_INFO info
    {
        .szName = name,
        .dwThreadID = GetCurrentThreadId()
    };

    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { }

#else

    //
    // Modern thread naming for Windows 10 1607 or later.
    // This method allows for thread names to be written to crash dumps.
    //

    // I doubt we'll ever need Unicode in thread names.
    const auto nameView = std::string_view(name);
    const auto nameW = std::wstring(nameView.begin(), nameView.end());

    SetThreadDescription(GetCurrentThread(), nameW.c_str());

#endif // defined(NTDDI_VERSION) && (NTDDI_VERSION < NTDDI_WIN10_RS1)

#else

#if defined(__linux__)

    // Linux limits thread names to 15 characters, and will
    // reject any attempt to set a longer name with ERANGE.
    const auto nameView = std::string_view(name, std::min(strlen(name), size_t(15)));

    pthread_setname_np(pthread_self(), nameView.data());

#elif defined(__APPLE__)

    // macOS does not take in a thread handle for this function.
    // The thread name is automatically truncated to 64 characters.
    pthread_setname_np(name);

#endif

#endif // defined(_WIN32)
}

// Sonicteam::SoX::Thread::ThreadProc
PPC_FUNC_IMPL(__imp__sub_825866A8);
PPC_FUNC(sub_825866A8)
{
    GuestThread::SetThreadName(reinterpret_cast<Sonicteam::SoX::Thread*>(base + ctx.r3.u32)->m_pName.get());

    __imp__sub_825866A8(ctx, base);
}

uint32_t XSetThreadProcessor(GuestThreadHandle* hThread, uint32_t dwIdealProcessor)
{
    return 0;
}

GUEST_FUNCTION_HOOK(sub_82537F80, XSetThreadProcessor);
