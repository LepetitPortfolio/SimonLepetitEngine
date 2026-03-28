#pragma once
#include "../PlatformConfig.h"

#if PLATFORM_LINUX

#include "../KeyboardPlatform.h"
#include "../../Common/EnumArray.h"
#include "../../Common/String.h"

#include <array>
#include <string>
#include <unordered_map>



struct KeyboardUnix_cls : public KeyboardPlatform
{
public:
    bool KeyPressed_str(Keyboard _Key);

    bool KeyPressed_str(ScanCode _ScanCode);

    ScanCode Delocalize(Keyboard _Key);

    Keyboard Localize(ScanCode _ScanCode);

    String GetDescription(ScanCode _ScanCode);

    Keyboard GetKeyFromEvent(XKeyEvent& _KeyEvent);

    ScanCode GetScancodeFromEvent(XKeyEvent& _KeyEvent);

private:

    KeyCode m_NullKeyCode = 0;
    int m_MaxKeyCode = 256;
    EnumArray<ScanCode, KeyCode, ScanCodeCount> m_ScancodeToKeyCode;
    std::array<ScanCode, m_MaxKeyCode> m_KeycodeToScancode;

    std::unordered_map<std::string, ScanCode> GetNameScancodeMap();

    bool IsValidKeycode(KeyCode _KeyCode);

    ScanCode TranslateKeyCode(Display* _Display, KeyCode _KeyCode);

    void EnsureMapping();

    KeyCode KeyToKeyCode(Keyboard _Key);

    KeyCode ScancodeToKeyCode(ScanCode _ScanCode);

    ScanCode KeyCodeToScancode(KeyCode _KeyCode);

    KeySym ScancodeToKeySym(ScanCode _ScanCode);

    bool KeyPressed_str(KeyCode _KeyCode);
};
#endif
