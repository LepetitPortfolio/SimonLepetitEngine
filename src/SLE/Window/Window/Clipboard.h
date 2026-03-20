#pragma once
#include "../../Platform.h"
#include "../../Common/String.h"

class Clipboard
{
public:
   
    static String GetString();

    static void SetString(const String& _Text);
};

#if PLATFORM_WINDOWS
#include "Win/ClipboardWin32.h"
using ClipboardType = ClipboardWin32;
#elif PLATFORM_LINUX
#include "Unix/ClipboardUnix.h"
using ClipboardType = ClipboardUnix;
#endif