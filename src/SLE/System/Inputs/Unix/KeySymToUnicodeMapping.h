#pragma once
#include "../../PlatformConfig.h"

#if PLATFORM_LINUX

#include <cstdint>

struct KeySymToUnicodeMapping
{
    char32_t KeysymToUnicode(KeySym _KeySym);
};


#endif