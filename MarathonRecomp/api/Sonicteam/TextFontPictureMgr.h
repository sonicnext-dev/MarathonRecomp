#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class TextFontPictureMgr : public SoX::IResourceMgr, public System::Singleton<TextFontPictureMgr, 0x82D3C38C, System::CreateStatic<TextFontPictureMgr, 0x8263BEF0>> {};

    MARATHON_ASSERT_SIZEOF(TextFontPictureMgr, 0x10);
}
