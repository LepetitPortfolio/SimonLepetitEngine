#pragma once
#include "../../../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "../Clipboard_cls.h"

class ClipboardWin32_cls : Clipboard_cls
{
public:

    static String GetString();

    static void SetString(const String& _Text);
};

#endif