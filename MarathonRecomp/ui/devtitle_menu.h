#pragma once

#include <api/Marathon.h>

class DevTitleMenu
{
public:

    static bool IsVisible;

    static void Init();
    static void Draw();

    static void Close();
    static bool CanClose();
};
