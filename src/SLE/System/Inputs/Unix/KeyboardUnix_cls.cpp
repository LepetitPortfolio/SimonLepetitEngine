#include "../../PlatformConfig.h"

#if PLATFORM_LINUX
#include "KeyboardUnix_cls.h"

#include "../../Window_cls/Unix/Display.h"
#include "KeySymToKeyMapping.h"
#include "KeySymToUnicodeMapping.h"


#include <array>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>


bool KeyboardUnix_cls::KeyPressed_str(Keyboard _Key)
{
	const KeyCode keycode = KeyToKeyCode(_Key);
	return KeyPressed_str(keycode);
}


bool KeyboardUnix_cls::KeyPressed_str(ScanCode _ScaneCode)
{
    const KeyCode keycode = ScancodeToKeyCode(_ScaneCode);
	return KeyPressed_str(keycode);
}


ScanCode KeyboardUnix_cls::Delocalize(Keyboard _Key)
{
    const KeyCode keycode = KeyToKeyCode(_Key);
    return KeyCodeToScancode(keycode);
}

Keyboard KeyboardUnix_cls::Localize(ScanCode _ScanCode)
{
    const KeySym keySym = ScancodeToKeySym(_ScanCode);
    KeySymToKeyMapping keySymToKeyMapping{};
    return keySymToKeyMapping.KeySymToKey(keySym);
}

String KeyboardUnix_cls::GetDescription(ScanCode _ScanCode)
{
    bool checkInput = true;

    // these scancodes actually correspond to keys with input
    // but we want to return their description, not their behaviour
    // clang-format off
    if (_ScanCode == ScanCode::Enter || _ScanCode == ScanCode::Escape || _ScanCode == ScanCode::Backspace || _ScanCode == ScanCode::Tab ||
        _ScanCode == ScanCode::Space || _ScanCode == ScanCode::ScrollLock || _ScanCode == ScanCode::Pause || _ScanCode == ScanCode::Delete ||
        _ScanCode == ScanCode::NumpadDivide || _ScanCode == ScanCode::NumpadMultiply || _ScanCode == ScanCode::NumpadMinus || _ScanCode == ScanCode::NumpadPlus ||
        _ScanCode == ScanCode::NumpadEqual || _ScanCode == ScanCode::NumpadEnter || _ScanCode == ScanCode::NumpadDecimal)
    {
        checkInput = false;
    }

    if (checkInput)
    {
        KeySymToUnicodeMapping keySymToUnicodeMapping{};
        const KeySym keysym = ScancodeToKeySym(_ScanCode);
        const char32_t unicode = keySymToUnicodeMapping.KeysymToUnicode(keysym);

        if (unicode != 0)
        {
            return {unicode};
        }
    }

    // Fallback to our best guess for the keys that are known to be independent of the layout.
    // clang-format off
    switch (_ScanCode)
    {
        case ScanCode::Enter:
        return "Enter";

        case ScanCode::Escape:
        return "Escape";

        case ScanCode::Backspace:
        return "Backspace";

        case ScanCode::Tab:
        return "Tab";

        case ScanCode::Space:
        return "Space";

        case ScanCode::F1:
        return "F1";

        case ScanCode::F2:
        return "F2";

        case ScanCode::F3:
        return "F3";

        case ScanCode::F4:
        return "F4";

        case ScanCode::F5:
        return "F5";

        case ScanCode::F6:
        return "F6";

        case ScanCode::F7:
        return "F7";

        case ScanCode::F8:
        return "F8";

        case ScanCode::F9:
        return "F9";

        case ScanCode::F10:
        return "F10";

        case ScanCode::F11:
        return "F11";

        case ScanCode::F12:
        return "F12";

        case ScanCode::F13:
        return "F13";

        case ScanCode::F14:
        return "F14";

        case ScanCode::F15:
        return "F15";

        case ScanCode::F16:
        return "F16";

        case ScanCode::F17:
        return "F17";

        case ScanCode::F18:
        return "F18";

        case ScanCode::F19:
        return "F19";

        case ScanCode::F20:
        return "F20";

        case ScanCode::F21:
        return "F21";

        case ScanCode::F22:
        return "F22";

        case ScanCode::F23:
        return "F23";

        case ScanCode::F24:
        return "F24";

        case ScanCode::CapsLock:
        return "Caps Lock";

        case ScanCode::PrintScreen:
        return "Print Screen";

        case ScanCode::ScrollLock:
        return "Scroll Lock";

        case ScanCode::Pause:
        return "Pause";

        case ScanCode::Insert:
        return "Insert";

        case ScanCode::Home:
        return "Home";

        case ScanCode::PageUp:
        return "Page Up";

        case ScanCode::Delete:
        return "Delete";

        case ScanCode::End:
        return "End";

        case ScanCode::PageDown:
        return "Page Down";

        case ScanCode::Left:
        return "Left Arrow";

        case ScanCode::Right:
        return "Right Arrow";

        case ScanCode::Down:
        return "Down Arrow";

        case ScanCode::Up:
        return "Up Arrow";

        case ScanCode::NumLock:
        return "Num Lock";

        case ScanCode::NumpadDivide:
        return "Divide (Numpad)";

        case ScanCode::NumpadMultiply:
        return "Multiply (Numpad)";

        case ScanCode::NumpadMinus:
        return "Minus (Numpad)";

        case ScanCode::NumpadPlus:
        return "Plus (Numpad)";

        case ScanCode::NumpadEqual:
        return "Equal (Numpad)";

        case ScanCode::NumpadEnter:
        return "Enter (Numpad)";

        case ScanCode::NumpadDecimal:
        return "Decimal (Numpad)";

        case ScanCode::Numpad0:
        return "0 (Numpad)";

        case ScanCode::Numpad1:
        return "1 (Numpad)";

        case ScanCode::Numpad2:
        return "2 (Numpad)";

        case ScanCode::Numpad3:
        return "3 (Numpad)";

        case ScanCode::Numpad4:
        return "4 (Numpad)";

        case ScanCode::Numpad5:
        return "5 (Numpad)";

        case ScanCode::Numpad6:
        return "6 (Numpad)";

        case ScanCode::Numpad7:
        return "7 (Numpad)";

        case ScanCode::Numpad8:
        return "8 (Numpad)";

        case ScanCode::Numpad9:
        return "9 (Numpad)";

        case ScanCode::Application:
        return "Application";

        case ScanCode::Execute:
        return "Execute";

        case ScanCode::Help:
        return "Help";

        case ScanCode::Menu:
        return "Menu";

        case ScanCode::Select:
        return "Select";

        case ScanCode::Stop:
        return "Stop";

        case ScanCode::Redo:
        return "Redo";

        case ScanCode::Undo:
        return "Undo";

        case ScanCode::Cut:
        return "Cut";

        case ScanCode::Copy:
        return "Copy";

        case ScanCode::Paste:
        return "Paste";

        case ScanCode::Search:
        return "Search";

        case ScanCode::VolumeMute:
        return "Volume Mute";

        case ScanCode::VolumeUp:
        return "Volume Up";

        case ScanCode::VolumeDown:
        return "Volume Down";

        case ScanCode::LControl:
        return "Left Control";

        case ScanCode::LShift:
        return "Left Shift";

        case ScanCode::LAlt:
        return "Left Alt";

        case ScanCode::LSystem:
        return "Left System";

        case ScanCode::RControl:
        return "Right Control";

        case ScanCode::RShift:
        return "Right Shift";

        case ScanCode::RAlt:
        return "Right Alt";

        case ScanCode::RSystem:
        return "Right System";

        case ScanCode::LaunchApplication1:
        return "Launch Application 1";

        case ScanCode::LaunchApplication2: 
        return "Launch Application 2";

        case ScanCode::Favorites:
        return "Favorites";

        case ScanCode::Back:
        return "Back";

        case ScanCode::Forward:
        return "Forward";

        case ScanCode::MediaNextTrack:
        return "Media Next Track";

        case ScanCode::MediaPlayPause:
        return "Media Play Pause";

        case ScanCode::MediaPreviousTrack: 
        return "Media Previous Track";

        case ScanCode::MediaStop:
        return "Media Stop";

        case ScanCode::HomePage:
        return "Home Page";

        case ScanCode::Refresh:
        return "Refresh";

        case ScanCode::LaunchMail:
        return "Launch Mail";
        
        case ScanCode::LaunchMediaSelect:
        return "Launch Media Select";

        default:
        return "Unknown Scancode";
    }
}

Keyboard KeyboardUnix_cls::GetKeyFromEvent(XKeyEvent& _KeyEvent)
{
    Keyboard outKey = Keyboard::Unknown;
    int index = 0;
    while ((outKey == Keyboard::Unknown) && (index < 4))
    {
        KeySymToKeyMapping keySymToKeyMapping{};
        const KeySym keysym = XLookupKeysym(&_KeyEvent, index);
        const Keyboard key = keySymToKeyMapping.KeySymToKey(keysym);

        if(key != Keyboard::Unknown)
        {
            outKey = key;
        }
        else
        {
            index++;
        }
    }
    
    return outKey;
}

ScanCode KeyboardUnix_cls::GetScancodeFromEvent(XKeyEvent& _KeyEvent)
{
    return KeyCodeToScancode(static_cast<KeyCode>(_KeyEvent.keycode));
}

std::unordered_map<std::string, ScanCode> KeyboardUnix_cls::GetNameScancodeMap()
{
    std::unordered_map<std::string, ScanCode> mapping;

    mapping.try_emplace("LSGT", ScanCode::NonUsBackslash);

    mapping.try_emplace("TLDE", ScanCode::Grave);
    mapping.try_emplace("AE01", ScanCode::Num1);
    mapping.try_emplace("AE02", ScanCode::Num2);
    mapping.try_emplace("AE03", ScanCode::Num3);
    mapping.try_emplace("AE04", ScanCode::Num4);
    mapping.try_emplace("AE05", ScanCode::Num5);
    mapping.try_emplace("AE06", ScanCode::Num6);
    mapping.try_emplace("AE07", ScanCode::Num7);
    mapping.try_emplace("AE08", ScanCode::Num8);
    mapping.try_emplace("AE09", ScanCode::Num9);
    mapping.try_emplace("AE10", ScanCode::Num0);
    mapping.try_emplace("AE11", ScanCode::Hyphen);
    mapping.try_emplace("AE12", ScanCode::Equal);
    mapping.try_emplace("BKSP", ScanCode::Backspace);
    mapping.try_emplace("TAB", ScanCode::Tab);
    mapping.try_emplace("AD01", ScanCode::Q);
    mapping.try_emplace("AD02", ScanCode::W);
    mapping.try_emplace("AD03", ScanCode::E);
    mapping.try_emplace("AD04", ScanCode::R);
    mapping.try_emplace("AD05", ScanCode::T);
    mapping.try_emplace("AD06", ScanCode::Y);
    mapping.try_emplace("AD07", ScanCode::U);
    mapping.try_emplace("AD08", ScanCode::I);
    mapping.try_emplace("AD09", ScanCode::O);
    mapping.try_emplace("AD10", ScanCode::P);
    mapping.try_emplace("AD11", ScanCode::LBracket);
    mapping.try_emplace("AD12", ScanCode::RBracket);
    mapping.try_emplace("BKSL", ScanCode::Backslash);
    mapping.try_emplace("RTRN", ScanCode::Enter);

    mapping.try_emplace("CAPS", ScanCode::CapsLock);
    mapping.try_emplace("AC01", ScanCode::A);
    mapping.try_emplace("AC02", ScanCode::S);
    mapping.try_emplace("AC03", ScanCode::D);
    mapping.try_emplace("AC04", ScanCode::F);
    mapping.try_emplace("AC05", ScanCode::G);
    mapping.try_emplace("AC06", ScanCode::H);
    mapping.try_emplace("AC07", ScanCode::J);
    mapping.try_emplace("AC08", ScanCode::K);
    mapping.try_emplace("AC09", ScanCode::L);
    mapping.try_emplace("AC10", ScanCode::Semicolon);
    mapping.try_emplace("AC11", ScanCode::Apostrophe);
    mapping.try_emplace("AC12", ScanCode::Backslash);

    mapping.try_emplace("LFSH", ScanCode::LShift);
    mapping.try_emplace("AB01", ScanCode::Z);
    mapping.try_emplace("AB02", ScanCode::X);
    mapping.try_emplace("AB03", ScanCode::C);
    mapping.try_emplace("AB04", ScanCode::V);
    mapping.try_emplace("AB05", ScanCode::B);
    mapping.try_emplace("AB06", ScanCode::N);
    mapping.try_emplace("AB07", ScanCode::M);
    mapping.try_emplace("AB08", ScanCode::Comma);
    mapping.try_emplace("AB09", ScanCode::Period);
    mapping.try_emplace("AB10", ScanCode::Slash);
    mapping.try_emplace("RTSH", ScanCode::RShift);

    mapping.try_emplace("LCTL", ScanCode::LControl);
    mapping.try_emplace("LALT", ScanCode::LAlt);
    mapping.try_emplace("SPCE", ScanCode::Space);
    mapping.try_emplace("RCTL", ScanCode::RControl);
    mapping.try_emplace("RALT", ScanCode::RAlt);
    mapping.try_emplace("LVL3", ScanCode::RAlt);
    mapping.try_emplace("ALGR", ScanCode::RAlt);
    mapping.try_emplace("LWIN", ScanCode::LSystem);
    mapping.try_emplace("RWIN", ScanCode::RSystem);

    mapping.try_emplace("HYPR", ScanCode::Application);
    mapping.try_emplace("EXEC", ScanCode::Execute);
    mapping.try_emplace("MDSW", ScanCode::ModeChange);
    mapping.try_emplace("MENU", ScanCode::Menu);
    mapping.try_emplace("COMP", ScanCode::Menu);
    mapping.try_emplace("SELE", ScanCode::Select);

    mapping.try_emplace("ESC", ScanCode::Escape);
    mapping.try_emplace("FK01", ScanCode::F1);
    mapping.try_emplace("FK02", ScanCode::F2);
    mapping.try_emplace("FK03", ScanCode::F3);
    mapping.try_emplace("FK04", ScanCode::F4);
    mapping.try_emplace("FK05", ScanCode::F5);
    mapping.try_emplace("FK06", ScanCode::F6);
    mapping.try_emplace("FK07", ScanCode::F7);
    mapping.try_emplace("FK08", ScanCode::F8);
    mapping.try_emplace("FK09", ScanCode::F9);
    mapping.try_emplace("FK10", ScanCode::F10);
    mapping.try_emplace("FK11", ScanCode::F11);
    mapping.try_emplace("FK12", ScanCode::F12);

    mapping.try_emplace("PRSC", ScanCode::PrintScreen);
    mapping.try_emplace("SCLK", ScanCode::ScrollLock);
    mapping.try_emplace("PAUS", ScanCode::Pause);

    mapping.try_emplace("INS", ScanCode::Insert);
    mapping.try_emplace("HOME", ScanCode::Home);
    mapping.try_emplace("PGUP", ScanCode::PageUp);
    mapping.try_emplace("DELE", ScanCode::Delete);
    mapping.try_emplace("END", ScanCode::End);
    mapping.try_emplace("PGDN", ScanCode::PageDown);

    mapping.try_emplace("UP", ScanCode::Up);
    mapping.try_emplace("RGHT", ScanCode::Right);
    mapping.try_emplace("DOWN", ScanCode::Down);
    mapping.try_emplace("LEFT", ScanCode::Left);

    mapping.try_emplace("NMLK", ScanCode::NumLock);
    mapping.try_emplace("KPDV", ScanCode::NumpadDivide);
    mapping.try_emplace("KPMU", ScanCode::NumpadMultiply);
    mapping.try_emplace("KPSU", ScanCode::NumpadMinus);

    mapping.try_emplace("KP7", ScanCode::Numpad7);
    mapping.try_emplace("KP8", ScanCode::Numpad8);
    mapping.try_emplace("KP9", ScanCode::Numpad9);
    mapping.try_emplace("KPAD", ScanCode::NumpadPlus);
    mapping.try_emplace("KP4", ScanCode::Numpad4);
    mapping.try_emplace("KP5", ScanCode::Numpad5);
    mapping.try_emplace("KP6", ScanCode::Numpad6);
    mapping.try_emplace("KP1", ScanCode::Numpad1);
    mapping.try_emplace("KP2", ScanCode::Numpad2);
    mapping.try_emplace("KP3", ScanCode::Numpad3);
    mapping.try_emplace("KPEN", ScanCode::NumpadEnter);
    mapping.try_emplace("KP0", ScanCode::Numpad0);
    mapping.try_emplace("KPDL", ScanCode::NumpadDecimal);
    mapping.try_emplace("KPEQ", ScanCode::NumpadEqual);

    mapping.try_emplace("FK13", ScanCode::F13);
    mapping.try_emplace("FK14", ScanCode::F14);
    mapping.try_emplace("FK15", ScanCode::F15);
    mapping.try_emplace("FK16", ScanCode::F16);
    mapping.try_emplace("FK17", ScanCode::F17);
    mapping.try_emplace("FK18", ScanCode::F18);
    mapping.try_emplace("FK19", ScanCode::F19);
    mapping.try_emplace("FK20", ScanCode::F20);
    mapping.try_emplace("FK21", ScanCode::F21);
    mapping.try_emplace("FK22", ScanCode::F22);
    mapping.try_emplace("FK23", ScanCode::F23);
    mapping.try_emplace("FK24", ScanCode::F24);
    mapping.try_emplace("LMTA", ScanCode::LSystem);
    mapping.try_emplace("RMTA", ScanCode::RSystem);
    mapping.try_emplace("MUTE", ScanCode::VolumeMute);
    mapping.try_emplace("VOL-", ScanCode::VolumeDown);
    mapping.try_emplace("VOL+", ScanCode::VolumeUp);
    mapping.try_emplace("STOP", ScanCode::Stop);
    mapping.try_emplace("REDO", ScanCode::Redo);
    mapping.try_emplace("AGAI", ScanCode::Redo);
    mapping.try_emplace("UNDO", ScanCode::Undo);
    mapping.try_emplace("COPY", ScanCode::Copy);
    mapping.try_emplace("PAST", ScanCode::Paste);
    mapping.try_emplace("FIND", ScanCode::Search);
    mapping.try_emplace("CUT", ScanCode::Cut);
    mapping.try_emplace("HELP", ScanCode::Help);

    mapping.try_emplace("I156", ScanCode::LaunchApplication1);
    mapping.try_emplace("I157", ScanCode::LaunchApplication2);
    mapping.try_emplace("I164", ScanCode::Favorites);
    mapping.try_emplace("I166", ScanCode::Back);
    mapping.try_emplace("I167", ScanCode::Forward);
    mapping.try_emplace("I171", ScanCode::MediaNextTrack);
    mapping.try_emplace("I172", ScanCode::MediaPlayPause);
    mapping.try_emplace("I173", ScanCode::MediaPreviousTrack);
    mapping.try_emplace("I174", ScanCode::MediaStop);
    mapping.try_emplace("I180", ScanCode::HomePage);
    mapping.try_emplace("I181", ScanCode::Refresh);
    mapping.try_emplace("I223", ScanCode::LaunchMail);
    mapping.try_emplace("I234", ScanCode::LaunchMediaSelect);

    return mapping;
}

bool KeyboardUnix_cls::IsValidKeycode(KeyCode _KeyCode)
{
    return _KeyCode >= 8;
}

ScanCode KeyboardUnix_cls::TranslateKeyCode(Display *_Display, KeyCode _KeyCode)
{
    if (!IsValidKeycode(_KeyCode))
    {
        return ScanCode::Unknown;
    }

    KeySym keySym = XkbKeycodeToKeysym(_Display, _KeyCode, 0, 1);

    switch (keySym)
    {
        case XK_KP_0:
        return ScanCode::Numpad0;

        case XK_KP_1:
        return ScanCode::Numpad1;

        case XK_KP_2:
        return ScanCode::Numpad2;

        case XK_KP_3:
        return ScanCode::Numpad3;

        case XK_KP_4:
        return ScanCode::Numpad4;

        case XK_KP_5:
        return ScanCode::Numpad5;

        case XK_KP_6:
        return ScanCode::Numpad6;

        case XK_KP_7:
        return ScanCode::Numpad7;

        case XK_KP_8:
        return ScanCode::Numpad8;

        case XK_KP_9:
        return ScanCode::Numpad9;

        case XK_KP_Separator:
        return ScanCode::NumpadDecimal;

        case XK_KP_Decimal:
        return ScanCode::NumpadDecimal;

        case XK_KP_Equal:
        return ScanCode::NumpadEqual;

        case XK_KP_Enter:
        return ScanCode::NumpadEnter;

        default:
        break;
    }
 
    keySym = XkbKeycodeToKeysym(_Display, _KeyCode, 0, 0);

    switch (keySym)
    {
        case XK_Return:
        return ScanCode::Enter;

        case XK_Escape:
        return ScanCode::Escape;

        case XK_BackSpace:
        return ScanCode::Backspace;

        case XK_Tab:
        return ScanCode::Tab;

        case XK_Shift_L:
        return ScanCode::LShift;

        case XK_Shift_R:
        return ScanCode::RShift;

        case XK_Control_L:
        return ScanCode::LControl;

        case XK_Control_R:
        return ScanCode::RControl;

        case XK_Alt_L:
        return ScanCode::LAlt;

        case XK_ISO_Level3_Shift: 
        case XK_Alt_R:
        return ScanCode::RAlt;

        case XK_Meta_L:
        case XK_Super_L:
        return ScanCode::LSystem;

        case XK_Meta_R:
        case XK_Super_R:
        return ScanCode::RSystem;

        case XK_Menu:
        return ScanCode::Menu;

        case XK_Num_Lock:
        return ScanCode::NumLock;

        case XK_Caps_Lock:
        return ScanCode::CapsLock;

        case XK_Execute:
        return ScanCode::Execute;

        case XK_Hyper_R:
        return ScanCode::Application;

        case XK_Select:
        return ScanCode::Select;

        case XK_Cancel:
        return ScanCode::Stop;

        case XK_Redo:
        return ScanCode::Redo;

        case XK_Undo:
        return ScanCode::Undo;

        case XK_Find:
        return ScanCode::Search;

        case XK_Mode_switch:
        return ScanCode::ModeChange;

        case XK_Print:
        return ScanCode::PrintScreen;

        case XK_Scroll_Lock:
        return ScanCode::ScrollLock;

        case XK_Pause:
        case XK_Break:
        return ScanCode::Pause;

        case XK_Delete:
        case XK_Clear:
        return ScanCode::Delete;

        case XK_Home:
        return ScanCode::Home;

        case XK_End:
        return ScanCode::End;

        case XK_Page_Up:
        return ScanCode::PageUp;

        case XK_Page_Down:
        return ScanCode::PageDown;

        case XK_Insert:
        return ScanCode::Insert;


        case XK_Left:
        return ScanCode::Left;

        case XK_Right:
        return ScanCode::Right;

        case XK_Down:
        return ScanCode::Down;

        case XK_Up:
        return ScanCode::Up;

        case XK_F1:
        return ScanCode::F1;

        case XK_F2:
        return ScanCode::F2;

        case XK_F3:
        return ScanCode::F3;

        case XK_F4:
        return ScanCode::F4;

        case XK_F5:
        return ScanCode::F5;

        case XK_F6:
        return ScanCode::F6;

        case XK_F7:
        return ScanCode::F7;

        case XK_F8:
        return ScanCode::F8;

        case XK_F9:
        return ScanCode::F9;

        case XK_F10:
        return ScanCode::F10;

        case XK_F11:
        return ScanCode::F11;

        case XK_F12:
        return ScanCode::F12;

        case XK_F13:
        return ScanCode::F13;

        case XK_F14:
        return ScanCode::F14;

        case XK_F15:
        return ScanCode::F15;

        case XK_F16:
        return ScanCode::F16;

        case XK_F17:
        return ScanCode::F17;

        case XK_F18:
        return ScanCode::F18;

        case XK_F19:
        return ScanCode::F19;

        case XK_F20:
        return ScanCode::F20;

        case XK_F21:
        return ScanCode::F21;

        case XK_F22:
        return ScanCode::F22;

        case XK_F23:
        return ScanCode::F23;

        case XK_F24:
        return ScanCode::F24;

        case XK_KP_Divide:
        return ScanCode::NumpadDivide;

        case XK_KP_Multiply:
        return ScanCode::NumpadMultiply;

        case XK_KP_Subtract:
        return ScanCode::NumpadMinus;

        case XK_KP_Add:
        return ScanCode::NumpadPlus;

        case XK_KP_Insert:
        return ScanCode::Numpad0;

        case XK_KP_End:
        return ScanCode::Numpad1;

        case XK_KP_Down:
        return ScanCode::Numpad2;

        case XK_KP_Page_Down:
        return ScanCode::Numpad3;

        case XK_KP_Left:
        return ScanCode::Numpad4;

        case XK_KP_Right:
        return ScanCode::Numpad6;

        case XK_KP_Home:
        return ScanCode::Numpad7;

        case XK_KP_Up:
        return ScanCode::Numpad8;

        case XK_KP_Page_Up:
        return ScanCode::Numpad9;

        case XK_KP_Delete:
        return ScanCode::NumpadDecimal;

        case XK_KP_Equal:
        return ScanCode::NumpadEqual;

        case XK_KP_Enter:
        return ScanCode::NumpadEnter;

        case XK_a:
        case XK_A:
        return ScanCode::A;

        case XK_b:
        case XK_B:
        return ScanCode::B;

        case XK_c:
        case XK_C:
        return ScanCode::C;

        case XK_d:
        case XK_D:
        return ScanCode::D;

        case XK_e:
        case XK_E:
        return ScanCode::E;

        case XK_f:
        case XK_F:
        return ScanCode::F;

        case XK_g:
        case XK_G:
        return ScanCode::G;

        case XK_h:
        case XK_H:
        return ScanCode::H;

        case XK_i:
        case XK_I:
        return ScanCode::I;

        case XK_j:
        case XK_J:
        return ScanCode::J;

        case XK_k:
        case XK_K:
        return ScanCode::K;

        case XK_l:
        case XK_L:
        return ScanCode::L;

        case XK_m:
        case XK_M:
        return ScanCode::M;

        case XK_n:
        case XK_N:
        return ScanCode::N;

        case XK_o:
        case XK_O:
        return ScanCode::O;

        case XK_p:
        case XK_P:
        return ScanCode::P;

        case XK_q:
        case XK_Q:
        return ScanCode::Q;

        case XK_r:
        case XK_R:
        return ScanCode::R;

        case XK_s:
        case XK_S:
        return ScanCode::S;

        case XK_t:
        case XK_T:
        return ScanCode::T;

        case XK_u:
        case XK_U:
        return ScanCode::U;

        case XK_v:
        case XK_V:
        return ScanCode::V;

        case XK_w:
        case XK_W:
        return ScanCode::W;

        case XK_x:
        case XK_X:
        return ScanCode::X;

        case XK_y:
        case XK_Y:
        return ScanCode::Y;

        case XK_z:
        case XK_Z:
        return ScanCode::Z;

        case XK_1:
        return ScanCode::Num1;

        case XK_2:
        return ScanCode::Num2;

        case XK_3:
        return ScanCode::Num3;

        case XK_4:
        return ScanCode::Num4;

        case XK_5:
        return ScanCode::Num5;

        case XK_6:
        return ScanCode::Num6;

        case XK_7:
        return ScanCode::Num7;

        case XK_8:
        return ScanCode::Num8;

        case XK_9:
        return ScanCode::Num9;

        case XK_0:
        return ScanCode::Num0;

        case XK_space:
        return ScanCode::Space;

        case XK_minus:
        return ScanCode::Hyphen;

        case XK_equal:
        return ScanCode::Equal;

        case XK_bracketleft:
        return ScanCode::LBracket;

        case XK_bracketright:
        return ScanCode::RBracket;

        case XK_backslash:
        return ScanCode::Backslash;

        case XK_semicolon:
        return ScanCode::Semicolon;

        case XK_apostrophe:
        return ScanCode::Apostrophe;

        case XK_grave:
        return ScanCode::Grave;

        case XK_comma:
        return ScanCode::Comma;

        case XK_period:
        return ScanCode::Period;

        case XK_slash:
        return ScanCode::Slash;

        case XK_less:
        return ScanCode::NonUsBackslash;

        default:
        return ScanCode::Unknown;
    }
    
}

void KeyboardUnix_cls::EnsureMapping()
{
    static bool isMappingInitialized = false;

    if(isMappingInitialized)
    {
        return;
    }

    m_ScancodeToKeyCode.fill(m_NullKeyCode);
    m_KeycodeToScancode.fill(ScanCode::Unknown);

    const auto display = OpenDisplay();

    std::array<char, XkbKeyNameLength + 1> name{};
    XkbDescPtr descriptor = XkbGetMap(display.get(), 0, XkbUseCoreKbd);
    XkbGetNames(display.get(), XkbKeyNamesMask, descriptor);

    std::unordered_map<std::string, ScanCode> nameScancodeMap =  GetNameScancodeMap();

    for(int keycode = descriptor->min_key_code; keycode < descriptor->max_key_code; keycode++)
    {
        if(!IsValidKeycode(static_cast<KeyCode>(keycode)))
        {
            continue;
        }

        std::memcpy(name.data(), descriptor->names->keys[keycode].name, XkbKeyNameLength);
        name[XkbKeyNameLength] = '\0';

        const auto mappedScancode = nameScancodeMap.find(std::string(name.data()));
        auto scancode = ScanCode::Unknown;

        if(mappedScancode != nameScancodeMap.end())
        {
            scancode = mappedScancode->second;
        }

        if(scancode != ScanCode::Unknown)
        {
            m_ScancodeToKeyCode[scancode] = static_cast<KeyCode>(keycode);
        }

        m_KeycodeToScancode[static_cast<KeyCode>(keycode)] = scancode;
    }

    XkbFreeNames(descriptor, XkbKeyNamesMask, True);
    XkbFreeKeyboard(descriptor, 0, True);

    for(int keycode = 8; keycode < m_MaxKeyCode; keycode++)
    {
        if(m_KeycodeToScancode[static_cast<KeyCode>(keycode)] == ScanCode::Unknown)
        {
            const auto scancode = TranslateKeyCode(display.get(), static_cast<KeyCode>(keycode));

            if(scancode != ScanCode::Unknown && m_ScancodeToKeyCode[scancode] == m_NullKeyCode)
            {
                m_ScancodeToKeyCode[scancode] = static_cast<KeyCode>(keycode);
            }

            m_KeycodeToScancode[static_cast<KeyCode>(keycode)] = scancode;
        }
    }

    isMappingInitialized = true;
}

KeyCode KeyboardUnix_cls::KeyToKeyCode(Keyboard _Key)
{
    KeySymToKeyMapping keySymToKeyMapping{};
    const KeySym keySym = keySymToKeyMapping.KeyToKeySym(_Key);

    if (keySym != NoSymbol)
    {
        const auto display = OpenDisplay();
        const KeyCode keyCode = XKeysymToKeycode(display.get(), keySym);

        if (keyCode != m_NullKeyCode)
        {
            return keyCode;
        }
    }

    if (_Key == Keyboard::RAlt)
    {
        return m_ScancodeToKeyCode[ScanCode::RAlt];
    }

    return m_NullKeyCode;
}

KeyCode KeyboardUnix_cls::ScancodeToKeyCode(ScanCode _ScanCode)
{
    EnsureMapping();

    if(_ScanCode != ScanCode::Unknown)
    {
        return m_ScancodeToKeyCode[_ScanCode];
    }

    return m_NullKeyCode;
}

ScanCode KeyboardUnix_cls::KeyCodeToScancode(KeyCode _KeyCode)
{
    EnsureMapping();

    if(IsValidKeycode(_KeyCode))
    {
        return m_KeycodeToScancode[_KeyCode];
    }

    return ScanCode::Unknown;
}

KeySym KeyboardUnix_cls::ScancodeToKeySym(ScanCode _ScanCode)
{
    const auto display = OpenDisplay();

    KeySym keysym = NoSymbol;
    const KeyCode keycode = ScancodeToKeyCode(_ScanCode);

    if(keycode != m_NullKeyCode)
    {
        keysym = XkbKeycodeToKeysym(display.get(), keycode, 0, 0);
    }

    return keysym;
}

bool KeyboardUnix_cls::KeyPressed_str(KeyCode _KeyCode)
{
    if(_KeyCode != m_NullKeyCode)
    {
        const auto display = OpenDisplay();

        std::array<char, 32> keys{};
        XQueryKeymap(display.get(), keys.data());

        return (keys[_KeyCode / 8] && (1 << (_KeyCode % 8))) != 0;
    }

    return false;
}

#endif


