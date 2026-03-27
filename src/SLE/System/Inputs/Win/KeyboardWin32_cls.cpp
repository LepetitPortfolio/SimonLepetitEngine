#include "../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "KeyboardWin32_cls.h"

Keyboard KeyboardWin32_cls::VirtualKeyToKey(UINT _VirtualKey)
{

	switch (_VirtualKey)
	{
	case 'A': return Keyboard::A;
	case 'B': return Keyboard::B;
	case 'C': return Keyboard::C;
	case 'D': return Keyboard::D;
	case 'E': return Keyboard::E;
	case 'F': return Keyboard::F;
	case 'G': return Keyboard::G;
	case 'H': return Keyboard::H;
	case 'I': return Keyboard::I;
	case 'J': return Keyboard::J;
	case 'K': return Keyboard::K;
	case 'L': return Keyboard::L;
	case 'M': return Keyboard::M;
	case 'N': return Keyboard::N;
	case 'O': return Keyboard::O;
	case 'P': return Keyboard::P;
	case 'Q': return Keyboard::Q;
	case 'R': return Keyboard::R;
	case 'S': return Keyboard::S;
	case 'T': return Keyboard::T;
	case 'U': return Keyboard::U;
	case 'V': return Keyboard::V;
	case 'W': return Keyboard::W;
	case 'X': return Keyboard::X;
	case 'Y': return Keyboard::Y;
	case 'Z': return Keyboard::Z;
	case '0': return Keyboard::Num0;
	case '1': return Keyboard::Num1;
	case '2': return Keyboard::Num2;
	case '3': return Keyboard::Num3;
	case '4': return Keyboard::Num4;
	case '5': return Keyboard::Num5;
	case '6': return Keyboard::Num6;
	case '7': return Keyboard::Num7;
	case '8': return Keyboard::Num8;
	case '9': return Keyboard::Num9;
	case VK_ESCAPE: return Keyboard::Escape;
	case VK_LCONTROL: return Keyboard::LControl;
	case VK_LSHIFT: return Keyboard::LShift;
	case VK_LMENU: return Keyboard::LAlt;
	case VK_LWIN: return Keyboard::LSystem;
	case VK_RCONTROL: return Keyboard::RControl;
	case VK_RSHIFT: return Keyboard::RShift;
	case VK_RMENU: return Keyboard::RAlt;
	case VK_RWIN: return Keyboard::RSystem;
	case VK_APPS: return Keyboard::Menu;
	case VK_OEM_4: return Keyboard::LBracket;
	case VK_OEM_6: return Keyboard::RBracket;
	case VK_OEM_1: return Keyboard::Semicolon;
	case VK_OEM_COMMA: return Keyboard::Comma;
	case VK_OEM_PERIOD: return Keyboard::Period;
	case VK_OEM_7: return Keyboard::Apostrophe;
	case VK_OEM_2: return Keyboard::Slash;
	case VK_OEM_5: return Keyboard::Backslash;
	case VK_OEM_3: return Keyboard::Grave;
	case VK_OEM_PLUS: return Keyboard::Equal;
	case VK_OEM_MINUS: return Keyboard::Hyphen;
	case VK_SPACE: return Keyboard::Space;
	case VK_RETURN: return Keyboard::Enter;
	case VK_BACK: return Keyboard::Backspace;
	case VK_TAB: return Keyboard::Tab;
	case VK_PRIOR: return Keyboard::PageUp;
	case VK_NEXT: return Keyboard::PageDown;
	case VK_END: return Keyboard::End;
	case VK_HOME: return Keyboard::Home;
	case VK_INSERT: return Keyboard::Insert;
	case VK_DELETE: return Keyboard::Delete;
	case VK_ADD: return Keyboard::Add;
	case VK_SUBTRACT: return Keyboard::Subtract;
	case VK_MULTIPLY: return Keyboard::Multiply;
	case VK_DIVIDE: return Keyboard::Divide;
	case VK_LEFT: return Keyboard::Left;
	case VK_RIGHT: return Keyboard::Right;
	case VK_UP: return Keyboard::Up;
	case VK_DOWN: return Keyboard::Down;
	case VK_NUMPAD0: return Keyboard::Numpad0;
	case VK_NUMPAD1: return Keyboard::Numpad1;
	case VK_NUMPAD2: return Keyboard::Numpad2;
	case VK_NUMPAD3: return Keyboard::Numpad3;
	case VK_NUMPAD4: return Keyboard::Numpad4;
	case VK_NUMPAD5: return Keyboard::Numpad5;
	case VK_NUMPAD6: return Keyboard::Numpad6;
	case VK_NUMPAD7: return Keyboard::Numpad7;
	case VK_NUMPAD8: return Keyboard::Numpad8;
	case VK_NUMPAD9: return Keyboard::Numpad9;
	case VK_F1: return Keyboard::F1;
	case VK_F2: return Keyboard::F2;
	case VK_F3: return Keyboard::F3;
	case VK_F4: return Keyboard::F4;
	case VK_F5: return Keyboard::F5;
	case VK_F6: return Keyboard::F6;
	case VK_F7: return Keyboard::F7;
	case VK_F8: return Keyboard::F8;
	case VK_F9: return Keyboard::F9;
	case VK_F10: return Keyboard::F10;
	case VK_F11: return Keyboard::F11;
	case VK_F12: return Keyboard::F12;
	case VK_F13: return Keyboard::F13;
	case VK_F14: return Keyboard::F14;
	case VK_F15: return Keyboard::F15;
	case VK_PAUSE: return Keyboard::Pause;
	default: return Keyboard::Unknown;
	}
}

int KeyboardWin32_cls::KeyToVirtualKey(Keyboard _Key)
{
	switch (_Key)
	{
	case Keyboard::A:  return 'A';
	case Keyboard::B:  return 'B';
	case Keyboard::C:  return 'C';
	case Keyboard::D:  return 'D';
	case Keyboard::E:  return 'E';
	case Keyboard::F:  return 'F';
	case Keyboard::G:  return 'G';
	case Keyboard::H:  return 'H';
	case Keyboard::I:  return 'I';
	case Keyboard::J:  return 'J';
	case Keyboard::K:  return 'K';
	case Keyboard::L:  return 'L';
	case Keyboard::M:  return 'M';
	case Keyboard::N:  return 'N';
	case Keyboard::O:  return 'O';
	case Keyboard::P:  return 'P';
	case Keyboard::Q:  return 'Q';
	case Keyboard::R:  return 'R';
	case Keyboard::S:  return 'S';
	case Keyboard::T:  return 'T';
	case Keyboard::U:  return 'U';
	case Keyboard::V:  return 'V';
	case Keyboard::W:  return 'W';
	case Keyboard::X:  return 'X';
	case Keyboard::Y:  return 'Y';
	case Keyboard::Z:  return 'Z';
	case Keyboard::Num0: return '0';
	case Keyboard::Num1: return '1';
	case Keyboard::Num2: return '2';
	case Keyboard::Num3: return '3';
	case Keyboard::Num4: return '4';
	case Keyboard::Num5: return '5';
	case Keyboard::Num6: return '6';
	case Keyboard::Num7: return '7';
	case Keyboard::Num8: return '8';
	case Keyboard::Num9: return '9';
	case Keyboard::Escape: return VK_ESCAPE;
	case Keyboard::LControl: return VK_LCONTROL;
	case Keyboard::LShift: return VK_LSHIFT;
	case Keyboard::LAlt: return VK_LMENU;
	case Keyboard::LSystem:  return VK_LWIN;
	case Keyboard::RControl: return VK_RCONTROL;
	case Keyboard::RShift: return VK_RSHIFT;
	case Keyboard::RAlt: return VK_RMENU;
	case Keyboard::RSystem:  return VK_RWIN;
	case Keyboard::Menu: return VK_APPS;
	case Keyboard::LBracket: return VK_OEM_4;
	case Keyboard::RBracket: return VK_OEM_6;
	case Keyboard::Semicolon:  return VK_OEM_1;
	case Keyboard::Comma:  return VK_OEM_COMMA;
	case Keyboard::Period: return VK_OEM_PERIOD;
	case Keyboard::Apostrophe: return VK_OEM_7;
	case Keyboard::Slash:  return VK_OEM_2;
	case Keyboard::Backslash:  return VK_OEM_5;
	case Keyboard::Grave:  return VK_OEM_3;
	case Keyboard::Equal:  return VK_OEM_PLUS;
	case Keyboard::Hyphen: return VK_OEM_MINUS;
	case Keyboard::Space:  return VK_SPACE;
	case Keyboard::Enter:  return VK_RETURN;
	case Keyboard::Backspace:  return VK_BACK;
	case Keyboard::Tab:  return VK_TAB;
	case Keyboard::PageUp: return VK_PRIOR;
	case Keyboard::PageDown: return VK_NEXT;
	case Keyboard::End:  return VK_END;
	case Keyboard::Home: return VK_HOME;
	case Keyboard::Insert: return VK_INSERT;
	case Keyboard::Delete: return VK_DELETE;
	case Keyboard::Add:  return VK_ADD;
	case Keyboard::Subtract: return VK_SUBTRACT;
	case Keyboard::Multiply: return VK_MULTIPLY;
	case Keyboard::Divide: return VK_DIVIDE;
	case Keyboard::Left: return VK_LEFT;
	case Keyboard::Right:  return VK_RIGHT;
	case Keyboard::Up: return VK_UP;
	case Keyboard::Down: return VK_DOWN;
	case Keyboard::Numpad0:  return VK_NUMPAD0;
	case Keyboard::Numpad1:  return VK_NUMPAD1;
	case Keyboard::Numpad2:  return VK_NUMPAD2;
	case Keyboard::Numpad3:  return VK_NUMPAD3;
	case Keyboard::Numpad4:  return VK_NUMPAD4;
	case Keyboard::Numpad5:  return VK_NUMPAD5;
	case Keyboard::Numpad6:  return VK_NUMPAD6;
	case Keyboard::Numpad7:  return VK_NUMPAD7;
	case Keyboard::Numpad8:  return VK_NUMPAD8;
	case Keyboard::Numpad9:  return VK_NUMPAD9;
	case Keyboard::F1: return VK_F1;
	case Keyboard::F2: return VK_F2;
	case Keyboard::F3: return VK_F3;
	case Keyboard::F4: return VK_F4;
	case Keyboard::F5: return VK_F5;
	case Keyboard::F6: return VK_F6;
	case Keyboard::F7: return VK_F7;
	case Keyboard::F8: return VK_F8;
	case Keyboard::F9: return VK_F9;
	case Keyboard::F10:  return VK_F10;
	case Keyboard::F11:  return VK_F11;
	case Keyboard::F12:  return VK_F12;
	case Keyboard::F13:  return VK_F13;
	case Keyboard::F14:  return VK_F14;
	case Keyboard::F15:  return VK_F15;
	case Keyboard::Pause:  return VK_PAUSE;
	default:  return 0;
	}
}

WORD KeyboardWin32_cls::ScanCodeToWinScan(ScanCode _ScanCode)
{
	switch (_ScanCode)
	{
	case ScanCode::A: return 0x1E;
	case ScanCode::B: return 0x30;
	case ScanCode::C: return 0x2E;
	case ScanCode::D: return 0x20;
	case ScanCode::E: return 0x12;
	case ScanCode::F: return 0x21;
	case ScanCode::G: return 0x22;
	case ScanCode::H: return 0x23;
	case ScanCode::I: return 0x17;
	case ScanCode::J: return 0x24;
	case ScanCode::K: return 0x25;
	case ScanCode::L: return 0x26;
	case ScanCode::M: return 0x32;
	case ScanCode::N: return 0x31;
	case ScanCode::O: return 0x18;
	case ScanCode::P: return 0x19;
	case ScanCode::Q: return 0x10;
	case ScanCode::R: return 0x13;
	case ScanCode::S: return 0x1F;
	case ScanCode::T: return 0x14;
	case ScanCode::U: return 0x16;
	case ScanCode::V: return 0x2F;
	case ScanCode::W: return 0x11;
	case ScanCode::X: return 0x2D;
	case ScanCode::Y: return 0x15;
	case ScanCode::Z: return 0x2C;

	case ScanCode::Num1: return 0x02;
	case ScanCode::Num2: return 0x03;
	case ScanCode::Num3: return 0x04;
	case ScanCode::Num4: return 0x05;
	case ScanCode::Num5: return 0x06;
	case ScanCode::Num6: return 0x07;
	case ScanCode::Num7: return 0x08;
	case ScanCode::Num8: return 0x09;
	case ScanCode::Num9: return 0x0A;
	case ScanCode::Num0: return 0x0B;

	case ScanCode::Enter:  return 0x1C;
	case ScanCode::Escape: return 0x01;
	case ScanCode::Backspace:  return 0x0E;
	case ScanCode::Tab:  return 0x0F;
	case ScanCode::Space:  return 0x39;
	case ScanCode::Hyphen: return 0x0C;
	case ScanCode::Equal:  return 0x0D;
	case ScanCode::LBracket: return 0x1A;
	case ScanCode::RBracket: return 0x1B;
	case ScanCode::Backslash:  return 0x2B;
	case ScanCode::Semicolon:  return 0x27;
	case ScanCode::Apostrophe: return 0x28;
	case ScanCode::Grave:  return 0x29;
	case ScanCode::Comma:  return 0x33;
	case ScanCode::Period: return 0x34;
	case ScanCode::Slash:  return 0x35;

	case ScanCode::F1:  return 0x3B;
	case ScanCode::F2:  return 0x3C;
	case ScanCode::F3:  return 0x3D;
	case ScanCode::F4:  return 0x3E;
	case ScanCode::F5:  return 0x3F;
	case ScanCode::F6:  return 0x40;
	case ScanCode::F7:  return 0x41;
	case ScanCode::F8:  return 0x42;
	case ScanCode::F9:  return 0x43;
	case ScanCode::F10: return 0x44;
	case ScanCode::F11: return 0x57;
	case ScanCode::F12: return 0x58;
	case ScanCode::F13: return 0x64;
	case ScanCode::F14: return 0x65;
	case ScanCode::F15: return 0x66;
	case ScanCode::F16: return 0x67;
	case ScanCode::F17: return 0x68;
	case ScanCode::F18: return 0x69;
	case ScanCode::F19: return 0x6A;
	case ScanCode::F20: return 0x6B;
	case ScanCode::F21: return 0x6C;
	case ScanCode::F22: return 0x6D;
	case ScanCode::F23: return 0x6E;
	case ScanCode::F24: return 0x76;

	case ScanCode::CapsLock:  return 0x3A;
	case ScanCode::PrintScreen: return 0xE037;
	case ScanCode::ScrollLock:  return 0x46;
	case ScanCode::Pause: return 0x45;
	case ScanCode::Insert:  return 0xE052;
	case ScanCode::Home:  return 0xE047;
	case ScanCode::PageUp:  return 0xE049;
	case ScanCode::Delete:  return 0xE053;
	case ScanCode::End: return 0xE04F;
	case ScanCode::PageDown:  return 0xE051;
	case ScanCode::Right: return 0xE04D;
	case ScanCode::Left:  return 0xE04B;
	case ScanCode::Down:  return 0xE050;
	case ScanCode::Up:  return 0xE048;
	case ScanCode::NumLock: return 0xE045;

	case ScanCode::NumpadDivide: return 0xE035;
	case ScanCode::NumpadMultiply: return 0x37;
	case ScanCode::NumpadMinus:  return 0x4A;
	case ScanCode::NumpadPlus: return 0x4E;
	case ScanCode::NumpadEqual:  return 0x7E;
	case ScanCode::NumpadEnter:  return 0xE01C;
	case ScanCode::NumpadDecimal:  return 0x53;

	case ScanCode::Numpad1:  return 0x4F;
	case ScanCode::Numpad2:  return 0x50;
	case ScanCode::Numpad3:  return 0x51;
	case ScanCode::Numpad4:  return 0x4B;
	case ScanCode::Numpad5:  return 0x4C;
	case ScanCode::Numpad6:  return 0x4D;
	case ScanCode::Numpad7:  return 0x47;
	case ScanCode::Numpad8:  return 0x48;
	case ScanCode::Numpad9:  return 0x49;
	case ScanCode::Numpad0:  return 0x52;

	case ScanCode::NonUsBackslash: return 0x56;
	case ScanCode::Help: return 0xE061;
	case ScanCode::Menu: return 0xE05D;
	case ScanCode::Select: return 0xE01E;

	case ScanCode::VolumeMute: return 0xE020;
	case ScanCode::VolumeUp: return 0xE02E;
	case ScanCode::VolumeDown: return 0xE02C;
	case ScanCode::MediaPlayPause: return 0xE022;
	case ScanCode::MediaStop:  return 0xE024;
	case ScanCode::MediaNextTrack: return 0xE019;
	case ScanCode::MediaPreviousTrack: return 0xE010;

	case ScanCode::LControl: return 0x1D;
	case ScanCode::LShift: return 0x2A;
	case ScanCode::LAlt: return 0x38;
	case ScanCode::LSystem:  return 0xE05B;
	case ScanCode::RControl: return 0xE01D;
	case ScanCode::RShift: return 0x36;
	case ScanCode::RAlt: return 0xE038;
	case ScanCode::RSystem:  return 0xE05C;

	case ScanCode::Back:  return 0xE06A;
	case ScanCode::Forward: return 0xE069;
	case ScanCode::Refresh: return 0xE067;
	case ScanCode::Stop:  return 0xE068;
	case ScanCode::Search:  return 0xE065;
	case ScanCode::Favorites: return 0xE066;
	case ScanCode::HomePage:  return 0xE030;

	case ScanCode::LaunchApplication1: return 0xE06B;
	case ScanCode::LaunchApplication2: return 0xE021;
	case ScanCode::LaunchMail: return 0xE06C;
	case ScanCode::LaunchMediaSelect:  return 0xE06D;

	default: return 0x0;
	}
}

WORD KeyboardWin32_cls::ScanCodeToWinScanExtended(ScanCode _ScanCode)
{
	switch (_ScanCode)
	{
	case ScanCode::PrintScreen:  return 55 | 0xE100;
	case ScanCode::Insert: return 82 | 0xE100;
	case ScanCode::Home: return 71 | 0xE100;
	case ScanCode::PageUp: return 73 | 0xE100;
	case ScanCode::Delete: return 83 | 0xE100;
	case ScanCode::End:  return 79 | 0xE100;
	case ScanCode::PageDown: return 81 | 0xE100;
	case ScanCode::Right:  return 77 | 0xE100;
	case ScanCode::Left: return 75 | 0xE100;
	case ScanCode::Down: return 80 | 0xE100;
	case ScanCode::Up: return 72 | 0xE100;
	case ScanCode::NumLock:  return 69 | 0xE100;
	case ScanCode::NumpadEnter:  return 28 | 0xE100;
	case ScanCode::NumpadDivide: return 53 | 0xE100;
	case ScanCode::Help: return 97 | 0xE100;
	case ScanCode::Menu: return 93 | 0xE100;
	case ScanCode::Select: return 30 | 0xE100;
	case ScanCode::VolumeMute: return 32 | 0xE100;
	case ScanCode::VolumeUp: return 46 | 0xE100;
	case ScanCode::VolumeDown: return 44 | 0xE100;
	case ScanCode::MediaPlayPause: return 34 | 0xE100;
	case ScanCode::MediaStop:  return 36 | 0xE100;
	case ScanCode::MediaNextTrack: return 25 | 0xE100;
	case ScanCode::MediaPreviousTrack: return 16 | 0xE100;
	case ScanCode::LSystem:  return 91 | 0xE100;
	case ScanCode::RControl: return 29 | 0xE100;
	case ScanCode::RAlt: return 56 | 0xE100;
	case ScanCode::RSystem:  return 92 | 0xE100;
	case ScanCode::Back: return 106 | 0xE100;
	case ScanCode::Forward:  return 105 | 0xE100;
	case ScanCode::Refresh:  return 103 | 0xE100;
	case ScanCode::Stop: return 104 | 0xE100;
	case ScanCode::Search: return 101 | 0xE100;
	case ScanCode::Favorites:  return 102 | 0xE100;
	case ScanCode::HomePage: return 48 | 0xE100;
	case ScanCode::LaunchApplication1: return 107 | 0xE100;
	case ScanCode::LaunchApplication2: return 33 | 0xE100;
	case ScanCode::LaunchMail: return 108 | 0xE100;
	case ScanCode::LaunchMediaSelect:  return 109 | 0xE100;

	default: return ScanCodeToWinScan(_ScanCode);
	}
}

UINT KeyboardWin32_cls::ScanCodeToVirtualKey(ScanCode _ScanCode)
{
	const WORD winScanCode = ScanCodeToWinScan(_ScanCode);

	switch (_ScanCode)
	{
	case ScanCode::Numpad0: return VK_NUMPAD0;
	case ScanCode::Numpad1: return VK_NUMPAD1;
	case ScanCode::Numpad2: return VK_NUMPAD2;
	case ScanCode::Numpad3: return VK_NUMPAD3;
	case ScanCode::Numpad4: return VK_NUMPAD4;
	case ScanCode::Numpad5: return VK_NUMPAD5;
	case ScanCode::Numpad6: return VK_NUMPAD6;
	case ScanCode::Numpad7: return VK_NUMPAD7;
	case ScanCode::Numpad8: return VK_NUMPAD8;
	case ScanCode::Numpad9: return VK_NUMPAD9;
	case ScanCode::NumpadMinus: return VK_SUBTRACT;
	case ScanCode::NumpadDecimal: return VK_DECIMAL;
	case ScanCode::NumpadDivide: return VK_DIVIDE;
	case ScanCode::Pause: return VK_PAUSE;
	case ScanCode::RControl: return VK_RCONTROL;
	case ScanCode::RAlt: return VK_RMENU;
	default:  return MapVirtualKey(winScanCode, MAPVK_VSC_TO_VK_EX);
	}
}

std::optional<std::string> KeyboardWin32_cls::ScanCodeToConsumerKeyName(ScanCode _ScanCode)
{
	switch (_ScanCode)
	{
	case ScanCode::MediaNextTrack: return "Next Track";
	case ScanCode::MediaPreviousTrack: return "Previous Track";
	case ScanCode::MediaStop:  return "Stop";
	case ScanCode::MediaPlayPause: return "Play/Pause";
	case ScanCode::VolumeMute: return "Mute";
	case ScanCode::VolumeUp: return "Volume Increment";
	case ScanCode::VolumeDown: return "Volume Decrement";
	case ScanCode::LaunchMediaSelect:  return "Consumer Control Configuration";
	case ScanCode::LaunchMail: return "Email Reader";
	case ScanCode::LaunchApplication2: return "Calculator";
	case ScanCode::LaunchApplication1: return "Local Machine Browser";
	case ScanCode::Search: return "Search";
	case ScanCode::HomePage: return "Home";
	case ScanCode::Back: return "Back";
	case ScanCode::Forward:  return "Forward";
	case ScanCode::Stop: return "Stop";
	case ScanCode::Refresh:  return "Refresh";
	case ScanCode::Favorites:  return "Bookmarks";

	default: return std::nullopt;
	}
}

bool KeyboardWin32_cls::IsValidKey(Keyboard _Key)
{
	return _Key > Keyboard::Unknown && static_cast<unsigned int>(_Key) < KeyCount;
}

bool KeyboardWin32_cls::IsValidScanCode(ScanCode _ScanCode)
{
	return _ScanCode > ScanCode::Unknown && static_cast<unsigned int>(_ScanCode) < ScanCodeCount;
}

void KeyboardWin32_cls::EnsureMapping()
{
	static bool mappingInitialized = false;

	if (mappingInitialized)
	{
		return;
	}

	m_KeyToScanCodeMapping.fill(ScanCode::Unknown);
	m_ScanCodeToKeyMapping.fill(Keyboard::Unknown);

	for (unsigned int i = 0; i < ScanCodeCount; ++i)
	{
		const auto scanCode = static_cast<ScanCode>(i);
		const UINT virtualKey = ScanCodeToVirtualKey(scanCode);
		const Keyboard key = VirtualKeyToKey(virtualKey);
		
		if (key != Keyboard::Unknown && m_KeyToScanCodeMapping[key] == ScanCode::Unknown)
		{
			m_KeyToScanCodeMapping[key] = scanCode;
		}
		m_ScanCodeToKeyMapping[scanCode] = key;
	}

	mappingInitialized = true;
}

bool KeyboardWin32_cls::KeyPressed(Keyboard _Key)
{
	const int virtualKey = KeyToVirtualKey(_Key);
	return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

bool KeyboardWin32_cls::KeyPressed(ScanCode _ScaneCode)
{
	const int virtualKey = ScanCodeToVirtualKey(_ScaneCode);
	return (GetAsyncKeyState(static_cast<int>(virtualKey)) & KF_UP) != 0;
}

Keyboard KeyboardWin32_cls::GetKey(ScanCode _ScaneCode)
{
	if (!IsValidScanCode(_ScaneCode))
	{
		return Keyboard::Unknown;
	}

	EnsureMapping();

	return m_ScanCodeToKeyMapping[_ScaneCode];
}

ScanCode KeyboardWin32_cls::GetScanCode(Keyboard _Key)
{
	if (!IsValidKey(_Key))
	{
		return ScanCode::Unknown;
	}

	EnsureMapping();

	return m_KeyToScanCodeMapping[_Key];
}

String KeyboardWin32_cls::GetDescription(ScanCode _ScanCode)
{
	if (const auto consumerKeyName = ScanCodeToConsumerKeyName(_ScanCode))
	{
		return *consumerKeyName;
	}

	String outStr = "Unknown";

	WORD winCode = ScanCodeToWinScanExtended(_ScanCode);
	std::array<WCHAR, 1024> name{};

	if ((winCode >= 0x64) && (winCode <= 0x6E))
	{
		winCode += 0x18;
	}

	if (winCode == 0x76)
	{
		winCode = 0x87;
	}

	if (GetKeyNameTextW(winCode << 16, name.data(), static_cast<int>(name.size())) > 0)
	{
		outStr = *name.data();
	}

	return outStr;

}

void KeyboardWin32_cls::SetVirtualKeyboardVisible(bool _Visible)
{
	//Not Applicable on Win32
}

#endif