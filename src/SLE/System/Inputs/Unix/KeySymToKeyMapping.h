#pragma once
#include "../../../PlatformConfig.h"

#if PLATFORM_LINUX

#include "../Keyboard.h"

struct KeySymToKeyMapping
{
    Keyboard KeySymToKey(KeySym _Symbol);

    KeySym KeyToKeySym(Keyboard _Key);
};

#endif