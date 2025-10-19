#include "text_patches.h"
#include <kernel/heap.h>
#include <kernel/memory.h>
#include <os/logger.h>
#include <user/config.h>

// Load text card.
PPC_FUNC_IMPL(__imp__sub_825ECB48);
PPC_FUNC(sub_825ECB48)
{
    auto* pMessage = (const char*)(base + ctx.r5.u32);
    void* pNewMessage = nullptr;

    for (auto& redirect : TextPatches::s_redirectedMessages)
    {
        if (HashStr(pMessage) != redirect.first)
            continue;

        pNewMessage = g_userHeap.Alloc(strlen(redirect.second) + 1);

        strcpy((char*)pNewMessage, redirect.second);

        ctx.r5.u32 = g_memory.MapVirtual(pNewMessage);
    }

    __imp__sub_825ECB48(ctx, base);

    if (!pNewMessage)
        return;

    g_userHeap.Free(pNewMessage);
}

// Sonicteam::GameImp::OpenHintWindow (speculatory)
PPC_FUNC_IMPL(__imp__sub_82173838);
PPC_FUNC(sub_82173838)
{
    if (!Config::Hints)
    {
        auto pMessage = (const char*)(base + ctx.r4.u32);

        // Block specific hints from volumes.
        for (auto& pattern : TextPatches::s_hintPatterns)
        {
            if (strcmpWildcard(pMessage, pattern))
            {
                LOGFN_UTILITY("Blocked hint: {}", pMessage);
                return;
            }
        }
    }

    __imp__sub_82173838(ctx, base);
}
