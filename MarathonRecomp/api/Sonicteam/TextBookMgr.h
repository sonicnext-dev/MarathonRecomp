#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class TextBookMgr : public SoX::IResourceMgr, public System::Singleton<TextBookMgr, 0x82D35ED8, System::CreateStatic<TextBookMgr, 0x82162D70>> {};
}
