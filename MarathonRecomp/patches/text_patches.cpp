#include "text_patches.h"
#include <api/Marathon.h>
#include <hid/hid.h>
#include <kernel/heap.h>
#include <kernel/memory.h>
#include <os/logger.h>
#include <user/config.h>
#include <app.h>

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

    static char16_t* s_replacementStringPool{};
    static ELanguage s_replacementStringPoolLanguage{};
    static bool s_usePS3TypoFixes{};

    auto& replacedMessages = Config::UsePS3TypoFixes ? TextPatches::s_replacedMessagesPS3 : TextPatches::s_replacedMessages;

    if (s_replacementStringPoolLanguage != Config::Language || s_usePS3TypoFixes != Config::UsePS3TypoFixes)
    {
        auto length = 0;

        // Compute string pool length.
        for (auto& replacement : replacedMessages)
            length += (Localise(replacement.second.pKey).length() * 2) + 2;

        if (s_replacementStringPool)
            g_userHeap.Free(s_replacementStringPool);

        s_replacementStringPool = reinterpret_cast<char16_t*>(g_userHeap.Alloc(length));
        s_replacementStringPoolLanguage = Config::Language;
        s_usePS3TypoFixes = Config::UsePS3TypoFixes;

        auto stringPoolPos = s_replacementStringPool;

        for (auto& replacement : replacedMessages)
        {
            auto& message = Localise(replacement.second.pKey);
            auto  wideMessage = TransformUTF8ToWString(message);
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
    
    for (auto& replacement : replacedMessages)
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

    if (Config::GetPreferredControllerIcons() == EControllerIcons::PlayStation)
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

// Sonicteam::GameImp::ProcMsgTextCardAction
PPC_FUNC_IMPL(__imp__sub_82176910);
PPC_FUNC(sub_82176910)
{
    if (Config::GetPreferredControllerIcons() != EControllerIcons::PlayStation)
    {
        __imp__sub_82176910(ctx, base);
        return;
    }

    const auto pGame = reinterpret_cast<Sonicteam::GameImp*>(base + ctx.r3.u32);
    const auto pMsgTextCardAction = reinterpret_cast<Sonicteam::Message::Game::MsgTextCardAction*>(base + ctx.r4.u32);

    if (pMsgTextCardAction->Name == "sound")
    {
        for (auto& rParameter : pMsgTextCardAction->Parameters)
        {
            // Swap these sounds around, as they're configured for the wrong hints.
            // This is an error with the early version of "msg_hint_ps3" included
            // in the Xbox 360 version, and was patched on PlayStation 3.
            if (rParameter == "all06_a04ps_rg")
            {
                rParameter = "all06_a04ps_sd";
                break;
            }
            else if (rParameter == "all06_a04ps_sd")
            {
                rParameter = "all06_a04ps_rg";
                break;
            }
        }

        const auto pTextBookMgr = Sonicteam::TextBookMgr::GetInstance();

        for (const auto& [rPath, pResource] : pTextBookMgr->GetResources())
        {
            if (!strstr(rPath.c_str(), "msg_hint_ps3"))
                continue;

            const auto pTextBook = static_cast<Sonicteam::TextBook*>(pResource.get());

            // L1/L2 and R1/R2 are flipped on PS3, leading to
            // the voice lines being incorrect for these hints.
            static constexpr std::array<const char*, 4> blockedButtons =
            {
                "button_lb", "button_lt", "button_rb", "button_rt"
            };

            for (const auto& [rTextCardKey, spTextCard] : pTextBook->m_TextCards)
            {
                const auto pVariables = spTextCard->m_pVariables.get();

                if (!pVariables)
                    continue;

                if (rTextCardKey == "hint_all06_a04_sd" || rTextCardKey == "hint_all06_a04_rg")
                {
                    // Swap "button_x" with "button_a" for these hints.
                    // This is an error with the early version of "msg_hint_ps3" included
                    // in the Xbox 360 version, and was patched on PlayStation 3.
                    if (auto found = strstr(pVariables, "picture(button_x)"))
                        const_cast<char*>(found)[15] = 'a';

                    continue;
                }

                for (const auto& rParameter : pMsgTextCardAction->Parameters)
                {
                    if (!strstr(pVariables, rParameter.c_str()))
                        continue;

                    for (const auto& rButton : blockedButtons)
                    {
                        // Mutes the voice lines for PlayStation hints using L1/L2 and R1/R2.
                        // This should be configured to do the same for remapped actions in the future.
                        if (strstr(pVariables, rButton))
                        {
                            LOGFN_UTILITY("Blocked hint sound: {}", rTextCardKey.c_str());
                            return;
                        }
                    }
                }
            }
        }
    }

    __imp__sub_82176910(ctx, base);
}

// Sonicteam::GameImp::ProcMsgOpenHint
PPC_FUNC_IMPL(__imp__sub_82173838);
PPC_FUNC(sub_82173838)
{
    const auto pTextBookMgr = Sonicteam::TextBookMgr::GetInstance();
    const auto pMessage = reinterpret_cast<const char*>(base + ctx.r4.u32);

    bool isHintBlocked{};

    for (const auto& [rKey, rValue] : pTextBookMgr->GetResources())
    {
        const auto pTextBook = static_cast<Sonicteam::TextBook*>(rValue.get());
        const auto it = pTextBook->m_TextCards.find(pMessage);

        if (it == pTextBook->m_TextCards.end())
            continue;

        if (strstr(rKey.c_str(), "msg_hint_xenon") || strstr(rKey.c_str(), "msg_hint_ps3"))
        {
            // Block all controller hints.
            isHintBlocked = !Config::ControlTutorial;
            break;
        }
        else if (!Config::Hints)
        {
            // Block specific contextual hints.
            for (auto& pattern : TextPatches::s_hintPatterns)
            {
                if (strcmpWildcard(pMessage, pattern))
                {
                    isHintBlocked = true;
                    break;
                }
            }

            if (isHintBlocked)
                break;
        }
    }

    if (isHintBlocked)
    {
        LOGFN_UTILITY("Blocked hint: {}", pMessage);
        return;
    }

    __imp__sub_82173838(ctx, base);
}
