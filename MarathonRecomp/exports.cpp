#include "exports.h"
#include <apu/embedded_player.h>
#include <kernel/function.h>
#include <kernel/heap.h>
#include <app.h>

void Game_PlaySound(const char* pName)
{
     if (EmbeddedPlayer::s_isActive)
     {
         EmbeddedPlayer::Play(pName);
     }
     else
     {
         Game_PlaySound("system", pName);
     }
}

void Game_PlaySound(const char* pBankName, const char* pName)
{
    auto pBankNameGuest = g_userHeap.Alloc(strlen(pBankName) + 1);
    auto pNameGuest = g_userHeap.Alloc(strlen(pName) + 1);

    strcpy((char*)pBankNameGuest, pBankName);
    strcpy((char*)pNameGuest, pName);

    GuestToHostFunction<int>(sub_824C7868, App::s_pApp->m_pDoc->m_pRootTask.get(), pBankNameGuest, pNameGuest);

    g_userHeap.Free(pBankNameGuest);
    g_userHeap.Free(pNameGuest);
}
