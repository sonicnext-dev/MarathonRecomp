#include "exports.h"
#include <apu/embedded_player.h>
#include <kernel/function.h>
#include <kernel/heap.h>
#include <app.h>

void Game_PlaySound(const char* in_pName)
{
     if (EmbeddedPlayer::s_isActive)
     {
         EmbeddedPlayer::Play(in_pName);
     }
     else
     {
         Game_PlaySound("system", in_pName);
     }
}

void Game_PlaySound(const char* in_pSoundBankName, const char* in_pSoundName)
{
    if (!App::s_pApp || !App::s_pApp->m_pDoc)
        return;

    const auto pAudioPlayer = App::s_pApp->m_pDoc->m_spAudioPlayer.get();

    if (!pAudioPlayer)
        return;

    const auto soundBankId = pAudioPlayer->GetSoundBankID(in_pSoundBankName);

    if (soundBankId == -1)
        return;

    const auto soundId = pAudioPlayer->GetSoundID(soundBankId, in_pSoundName);

    if (soundId == -1)
        return;

    pAudioPlayer->CreateCue(soundId);
}
