#pragma once

#include <Marathon.inl>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam
{
    class TextBookMgr : public SoX::IResourceMgr, public System::Singleton<TextBookMgr, 0x82D35ED8, System::CreateStatic<TextBookMgr, 0x82162D70>> {};
}
