#pragma once
#include "../../../Platform.h"

#if PLATFORM_WINDOWS

#include "../Clipboard.h"

class ClipboardWin32 : Clipboard
{
public:

    static String GetString();

    static void SetString(const String& _Text);
};

#endif