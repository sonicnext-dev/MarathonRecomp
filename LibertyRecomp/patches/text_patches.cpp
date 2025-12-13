#include "text_patches.h"
#include <hid/hid.h>
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

// Load text book.
PPC_FUNC_IMPL(__imp__sub_821735B8);
PPC_FUNC(sub_821735B8)
{
    auto* pTextBookPath = (const char*)(base + ctx.r5.u32);
    void* pNewTextBookPath = nullptr;

    if (Config::IsControllerIconsPS3())
    {
        static constexpr const char* MSG_HINT_PS3 = "text/msg_hint_ps3.mst";

        if (strcmp(pTextBookPath, "text/msg_hint_xenon.mst") == 0)
        {
            pNewTextBookPath = g_userHeap.Alloc(strlen(MSG_HINT_PS3) + 1);

            strcpy((char*)pNewTextBookPath, MSG_HINT_PS3);

            ctx.r5.u32 = g_memory.MapVirtual(pNewTextBookPath);
        }
    }

    __imp__sub_821735B8(ctx, base);

    if (!pNewTextBookPath)
        return;

    g_userHeap.Free(pNewTextBookPath);
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
