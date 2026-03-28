#pragma once
#include "../PlatformConfig.h"
#include "../../Common/String.h"

class Clipboard_cls
{
public:
   
    static String GetString();

    static void SetString(const String& _Text);
};

#if PLATFORM_WINDOWS
#include "Win/ClipboardWin32_cls.h"
using ClipboardType = ClipboardWin32_cls;
#elif PLATFORM_LINUX
#include "Unix/ClipboardUnix_cls.h"
using ClipboardType = ClipboardUnix_cls;
#endif