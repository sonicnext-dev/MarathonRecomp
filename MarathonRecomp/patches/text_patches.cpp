#include "text_patches.h"
#include <api/Marathon.h>
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

    if (pNewMessage)
        g_userHeap.Free(pNewMessage);

    static uint16_t* s_replacementStringPool{};
    static ELanguage s_replacementStringPoolLanguage{};

    if (s_replacementStringPoolLanguage != Config::Language)
    {
        auto length = 0;

        // Compute string pool length.
        for (auto& replacement : TextPatches::s_replacedMessages)
            length += (Localise(replacement.second.pKey).length() * 2) + 2;

        if (s_replacementStringPool)
            g_userHeap.Free(s_replacementStringPool);

        s_replacementStringPool = (uint16_t*)g_userHeap.Alloc(length);
        s_replacementStringPoolLanguage = Config::Language;

        auto stringPoolPos = s_replacementStringPool;

        for (auto& replacement : TextPatches::s_replacedMessages)
        {
            auto& message = Localise(replacement.second.pKey);
            auto  wideMessage = std::wstring(message.begin(), message.end());
            auto  wideMessageLen = wideMessage.length() + 1;

            replacement.second.pGuestText = stringPoolPos;

            for (size_t i = 0; i < wideMessageLen; i++)
            {
                *stringPoolPos = ByteSwap(wideMessage.c_str()[i]);
                stringPoolPos++;
            }
        }
    }

    auto pspTextCard = (boost::shared_ptr<Sonicteam::TextCard>*)(base + ctx.r3.u32);
    
    for (auto& replacement : TextPatches::s_replacedMessages)
    {
        if (HashStr(pMessage) != replacement.first)
            continue;

        pspTextCard->get()->m_spResource = boost::make_shared<uint8_t>(0, 0x820334C4);
        pspTextCard->get()->m_pText = replacement.second.pGuestText;
        pspTextCard->get()->m_pVariables = (const char*)0x8200139C;
    }
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
