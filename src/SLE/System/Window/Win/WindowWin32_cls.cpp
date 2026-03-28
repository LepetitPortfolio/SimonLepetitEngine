#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "WindowWin32_cls.h"
#include <dbt.h>

#include "../../../Common/Error.h"
#include "../../../Common/UTF.h"

#include <assert.h>

WindowWin32_cls::WindowWin32_cls(WindowHandle _Handle) : m_WindowHandle(_Handle)
{
	SetProcessDpiAware();

	if (m_WindowHandle)
	{
		if (m_HandleCount == 0)
		{
			Joystick::SetLazyUpdates(true);

			InitRawMouse();
		}

		m_HandleCount++;

		SetWindowLongPtrW(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		m_Callback = SetWindowLongPtrW(m_WindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowWin32_cls::GlobalOnEvent));
	}
}

WindowWin32_cls::WindowWin32_cls(WindowConfig _WindowConfig, const std::string& _Title, std::uint32_t _Style, WindowState_e _State, const WindowSettings_str& _WindowSettings)
m_LastSize(_WindowConfig.m_Size), m_FullScreen(_State == WindowState_e::Fullscreen), m_Cursor_clsGrabbed(m_FullScreen)
{
	SetProcessDPIAware();

	if (m_WindowCount == 0)
	{
		RegisterWindowClass();
	}

	HDC screenDC = GetDC(nullptr);
	const int left = (GetDeviceCaps(screenDC, HORZRES) - static_cast<int>(_WindowConfig.m_Size.X)) / 2;
	const int top = (GetDeviceCaps(screenDC, VERTRES) - static_cast<int>(_WindowConfig.m_Size.Y)) / 2;
	auto [width, height] = Vector2i(_WindowConfig.m_Size);
	ReleaseDC(nullptr, screenDC);

	DWORD win32Style = WS_VISIBLE;
	if (_Style == WindowStyle_e::None)
	{
		win32Style |= WS_POPUP;
	}
	else
	{
		if (_Style & WindowStyle_e::Titlebar)
		{
			win32Style |= WS_CAPTION | WS_MINIMIZEBOX;
		}

		if (_Style & WindowStyle_e::Resize)
		{
			win32Style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
		}

		if (_Style & WindowStyle_e::Close)
		{
			win32Style |= WS_SYSMENU;
		}
	}

	if (!m_FullScreen)
	{
		RECT rectangle = { 0, 0, width, height };
		AdjustWindowRect(&rectangle, win32Style, false);
		width = rectangle.right - rectangle.left;
		height = rectangle.bottom - rectangle.top;
	}

	m_WindowHandle = CreateWindowW(m_ClassName, (LPCWSTR)_Title.c_str(), win32Style, left, top, width, height, nullptr, nullptr, GetModuleHandle(nullptr), this);

	DEV_BROADCAST_DEVICEINTERFACE deviceInterface = { sizeof(DEV_BROADCAST_DEVICEINTERFACE), DBT_DEVTYP_DEVICEINTERFACE, 0, m_GuidDevinterfaceHid , {0} };
	RegisterDeviceNotification(m_WindowHandle, &deviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);

	if (m_WindowHandle)
	{
		if (m_HandleCount == 0)
		{
			Joystick::SetLazyUpdate(true);

			InitRawMouse();
		}

		m_HandleCount++;
	}

	SetSize(_WindowConfig.m_Size);

	if (m_FullScreen)
	{
		SwitchToFullscreen(_WindowConfig);
	}

	m_WindowCount++;
}

WindowWin32_cls::~WindowWin32_cls()
{
	if (m_Icon)
	{
		DestroyIcon(m_Icon);
	}

	if (m_WindowHandle)
	{
		m_HandleCount--;

		if (m_HandleCount == 0)
		{
			Joystick::SetLazyUpdate(false);
		}
	}

	if (!m_Callback)
	{
		if (m_WindowHandle)
		{
			DestroyWindow(m_WindowHandle);
		}

		m_HandleCount--;

		if (m_HandleCount == 0)
		{
			UnregisterClassW(m_ClassName, GetModuleHandleW(nullptr));
		}
		else
		{
			SetWindowLongPtrW(m_WindowHandle, GWLP_WNDPROC, m_Callback);
		}
	}
}

void WindowWin32_cls::SetProcessDpiAware()
{
	if (const HINSTANCE shCoreDLL = LoadLibrary("Shcore.dll"))
	{
		enum ProcessDpiAwareness
		{
			ProcessDpiUnaware = 0,
			ProcessSystemDpiAware = 1,
			ProcessPerMonitorDpiAware = 2
		};

		using SetProcessDpiAwarenessFuncType = HRESULT(WINAPI*)(ProcessDpiAwareness);
		auto setProcessDpiAwarenessFunc = reinterpret_cast<SetProcessDpiAwarenessFuncType>(reinterpret_cast<void*>(GetProcAddress(shCoreDLL, "SetProcessDpiAwareness")));

		if (setProcessDpiAwarenessFunc)
		{
			if (setProcessDpiAwarenessFunc(ProcessPerMonitorDpiAware) == E_INVALIDARG)
			{
				Err() << "Failed to set process DPI awareness" << std::endl;
			}
			else
			{
				FreeLibrary(shCoreDLL);
				return;
			}
		}
		
		FreeLibrary(shCoreDLL);
	}

	if (const HINSTANCE user32DLL = LoadLibrary("user32.dll"))
	{
		using SetProcessDPIAwareFuncType = BOOL(WINAPI*)();
		auto setProcessDPIAwareFunc = reinterpret_cast<SetProcessDPIAwareFuncType>(reinterpret_cast<void*>(GetProcAddress(user32DLL, "SetProcessDPIAware")));

		if (setProcessDPIAwareFunc)
		{
			if (!setProcessDPIAwareFunc())
			{
				Err() << "Failed to set process DPI awareness" << std::endl;
			}
		}

		FreeLibrary(user32DLL);
	}
}

void WindowWin32_cls::InitRawMouse()
{
	const RAWINPUTDEVICE rawMouse{ 0x01, 0x02, 0, nullptr }; 

	if (RegisterRawInputDevices(&rawMouse, 1, sizeof(rawMouse)) != TRUE)
	{
		Err() << "Failed to initialize raw mouse input" << std::endl;
	}
}

void WindowWin32_cls::RegisterWindowClass()
{
	WNDCLASSW windowClass;
	windowClass.style = 0;
	windowClass.lpfnWndProc = &WindowWin32_cls::GlobalOnEvent;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleW(nullptr);
	windowClass.hIcon = nullptr;
	windowClass.hCursor_cls = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = m_ClassName;
	RegisterClassW(&windowClass);
}

void WindowWin32_cls::SwitchToFullscreen(const WindowConfig& _WindowConfig)
{
	DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);
	devMode.dmPelsWidth = _WindowConfig.m_Size.X;
	devMode.dmPelsHeight = _WindowConfig.m_Size.Y;
	devMode.dmBitsPerPel = _WindowConfig.m_BitsPerPixel;
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

	if (ChangeDisplaySettingsW(&devMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		Err() << "Failed to change display mode for fullscreen" << std::endl;
		return;
	}

	SetWindowLongPtr(m_WindowHandle, GWL_STYLE, static_cast<LONG_PTR>(WS_POPUP) | static_cast<LONG_PTR>(WS_CLIPCHILDREN) | static_cast<LONG_PTR>(WS_CLIPSIBLINGS));
	SetWindowLongPtr(m_WindowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW);

	SetWindowPos(m_WindowHandle, HWND_TOP, 0, 0, static_cast<int>(_WindowConfig.m_Size.X), static_cast<int>(_WindowConfig.m_Size.Y), SWP_FRAMECHANGED);
	ShowWindow(m_WindowHandle, SW_SHOW);

	m_FullScreenWindow = this;
}

void WindowWin32_cls::Cleanup()
{
	if (m_FullScreenWindow == this)
	{
		ChangeDisplaySettingsW(nullptr, 0);
		m_FullScreenWindow = nullptr;
	}

	SetCursor_clsVisible(true);

	SetTracking(false);

	ReleaseCapture();
}

void WindowWin32_cls::SetTracking(bool _Track)
{
	TRACKMOUSEEVENT mouseEvent;
	mouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	mouseEvent.dwFlags = _Track ? TME_LEAVE : TME_CANCEL;
	mouseEvent.hwndTrack = m_WindowHandle;
	mouseEvent.dwHoverTime = HOVER_DEFAULT;
	TrackMouseEvent(&mouseEvent);
}

void WindowWin32_cls::GrabCursor_cls(bool _Grabbed)
{
	if (_Grabbed)
	{
		RECT rect;
		GetClientRect(m_WindowHandle, &rect);
		MapWindowPoints(m_WindowHandle, nullptr, reinterpret_cast<LPPOINT>(&rect), 2);
		ClipCursor_cls(&rect);
	}
	else
	{
		ClipCursor_cls(nullptr);
	}
}

Vector2i WindowWin32_cls::ContentSizeToWindowSize(Vector2u _Size)
{
	const auto style = static_cast<DWORD>(GetWindowLongPtr(m_WindowHandle, GWL_STYLE));
	const BOOL hasMenu = ((style & WS_CHILD) == 0) && GetMenu(m_WindowHandle) != nullptr;
	const auto exStyle = static_cast<DWORD>(GetWindowLongPtr(m_WindowHandle, GWL_EXSTYLE));

	RECT rect = { 0, 0, static_cast<long>(_Size.X), static_cast<long>(_Size.Y) };
	AdjustWindowRectEx(&rect, style, hasMenu, exStyle);
	const auto width = rect.right - rect.left;
	const auto height = rect.bottom - rect.top;

	return {width, height};
}

ScanCode WindowWin32_cls::ToScancode(WPARAM _WParam, LPARAM _LParam)
{
	int code = (_LParam & (0xFF << 16)) >> 16;

	if (code == 0)
	{
		code = static_cast<int>(MapVirtualKey(static_cast<UINT>(_WParam), MAPVK_VK_TO_VSC));
	}

	switch (code)
	{
	case 1: return ScanCode::Escape;
	case 2: return ScanCode::Num1;
	case 3: return ScanCode::Num2;
	case 4: return ScanCode::Num3;
	case 5: return ScanCode::Num4;
	case 6: return ScanCode::Num5;
	case 7: return ScanCode::Num6;
	case 8: return ScanCode::Num7;
	case 9: return ScanCode::Num8;
	case 10: return ScanCode::Num9;
	case 11: return ScanCode::Num0;
	case 12: return ScanCode::Hyphen;
	case 13: return ScanCode::Equal;
	case 14: return ScanCode::Backspace;
	case 15: return ScanCode::Tab;
	case 16: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::MediaPreviousTrack : ScanCode::Q;
	case 17: return ScanCode::W;
	case 18: return ScanCode::E;
	case 19: return ScanCode::R;
	case 20: return ScanCode::T;
	case 21: return ScanCode::Y;
	case 22: return ScanCode::U;
	case 23: return ScanCode::I;
	case 24: return ScanCode::O;
	case 25: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::MediaNextTrack : ScanCode::P;
	case 26: return ScanCode::LBracket;
	case 27: return ScanCode::RBracket;
	case 28: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::NumpadEnter : ScanCode::Enter;
	case 29: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::RControl : ScanCode::LControl;
	case 30: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Select : ScanCode::A;
	case 31: return ScanCode::S;
	case 32: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::VolumeMute : ScanCode::D;
	case 33: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::LaunchApplication1 : ScanCode::F;
	case 34: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::MediaPlayPause : ScanCode::G;
	case 35: return ScanCode::H;
	case 36: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::MediaStop : ScanCode::J;
	case 37: return ScanCode::K;
	case 38: return ScanCode::L;
	case 39: return ScanCode::Semicolon;
	case 40: return ScanCode::Apostrophe;
	case 41: return ScanCode::Grave;
	case 42: return ScanCode::LShift;
	case 43: return ScanCode::Backslash;
	case 44: return ScanCode::Z;
	case 45: return ScanCode::X;
	case 46: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::VolumeDown : ScanCode::C;
	case 47: return ScanCode::V;
	case 48: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::VolumeUp : ScanCode::B;
	case 49: return ScanCode::N;
	case 50: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::HomePage : ScanCode::M;
	case 51: return ScanCode::Comma;
	case 52: return ScanCode::Period;
	case 53: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::NumpadDivide : ScanCode::Slash;
	case 54: return ScanCode::RShift;
	case 55: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::PrintScreen : ScanCode::NumpadMultiply;
	case 56: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::RAlt : ScanCode::LAlt;
	case 57: return ScanCode::Space;
	case 58: return ScanCode::CapsLock;
	case 59: return ScanCode::F1;
	case 60: return ScanCode::F2;
	case 61: return ScanCode::F3;
	case 62: return ScanCode::F4;
	case 63: return ScanCode::F5;
	case 64: return ScanCode::F6;
	case 65: return ScanCode::F7;
	case 66: return ScanCode::F8;
	case 67: return ScanCode::F9;
	case 68: return ScanCode::F10;
	case 69: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::NumLock : ScanCode::Pause;
	case 70: return ScanCode::ScrollLock;
	case 71: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Home : ScanCode::Numpad7;
	case 72: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Up : ScanCode::Numpad8;
	case 73: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::PageUp : ScanCode::Numpad9;
	case 74: return ScanCode::NumpadMinus;
	case 75: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Left : ScanCode::Numpad4;
	case 76: return ScanCode::Numpad5;
	case 77: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Right : ScanCode::Numpad6;
	case 78: return ScanCode::NumpadPlus;
	case 79: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::End : ScanCode::Numpad1;
	case 80: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Down : ScanCode::Numpad2;
	case 81: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::PageDown : ScanCode::Numpad3;
	case 82: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Insert : ScanCode::Numpad0;
	case 83: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Delete : ScanCode::NumpadDecimal;

	case 86: return ScanCode::NonUsBackslash;
	case 87: return ScanCode::F11;
	case 88: return ScanCode::F12;

	case 91: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::LSystem : ScanCode::Unknown;
	case 92: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::RSystem : ScanCode::Unknown;
	case 93: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Menu : ScanCode::Unknown;

	case 99: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Help : ScanCode::Unknown;
	case 100: return ScanCode::F13;
	case 101: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Search : ScanCode::F14;
	case 102: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Favorites : ScanCode::F15;
	case 103: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Refresh : ScanCode::F16;
	case 104: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Stop : ScanCode::F17;
	case 105: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Forward : ScanCode::F18;
	case 106: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::Back : ScanCode::F19;
	case 107: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::LaunchApplication1 : ScanCode::F20;
	case 108: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::LaunchMail : ScanCode::F21;
	case 109: return (HIWORD(_LParam) & KF_EXTENDED) ? ScanCode::LaunchMediaSelect : ScanCode::F22;
	case 110: return ScanCode::F23;

	case 118: return ScanCode::F24;

	default: return ScanCode::Unknown;
	}

	return ScanCode::Unknown;
}

void WindowWin32_cls::ProcessEvent(UINT _Message, WPARAM _WParam, LPARAM _LParam)
{
	if (m_WindowHandle == nullptr)
	{
		return;
	}

	switch (_Message)
	{
	case WM_DESTROY:
		Cleanup();
		break;

	case WM_SETCursor_cls:
		SetCursor_clsEvent(_LParam);
		break;

	case WM_CLOSE:
		PushEvent(Event_cls::Closed_str{});
		break;

	case WM_SIZE:
		ResizeEvent(_WParam);
		break;

	case WM_ENTERSIZEMOVE:
		m_Resizing = true;
		GrabCursor_cls(false);
		break;

	case WM_EXITSIZEMOVE:
		StopResizing();
		break;

	case WM_GETMINMAXINFO:
		GetMinMaxInfo(_LParam);
		break;

	case WM_SETFOCUS:
		GainFocusEvent();
		break;

	case WM_KILLFOCUS:
		LostFocusEvent();
		break;

	case WM_CHAR:
		TextEvent(_WParam, _LParam);
		break;
	
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		KeyDownEvent(_WParam, _LParam);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		KeyUpEvent(_WParam, _LParam);
		break;

	case WM_MOUSEWHEEL:
		MouseWheelEvent(MouseWheel_e::Vertical, _WParam, _LParam);
		break;

	case WM_MOUSEHWHEEL:
		MouseWheelEvent(MouseWheel_e::Horizontal, _WParam, _LParam);
		break;

	case WM_LBUTTONDOWN:
		MouseButtonDownEvent(MouseButton_e::Left, _LParam);
		break;

	case WM_LBUTTONUP:
		MouseButtonUpEvent(MouseButton_e::Left, _LParam);
		break;

	case WM_RBUTTONDOWN:
		MouseButtonDownEvent(MouseButton_e::Right, _LParam);
		break;

	case WM_RBUTTONUP:
		MouseButtonUpEvent(MouseButton_e::Right, _LParam);
		break;

	case WM_MBUTTONDOWN:
		MouseButtonDownEvent(MouseButton_e::Middle, _LParam);
		break;

	case WM_MBUTTONUP:
		MouseButtonUpEvent(MouseButton_e::Middle, _LParam);
		break;

	case WM_XBUTTONDOWN:
		MouseButtonDownEvent(HIWORD(_WParam) == XBUTTON1 ? MouseButton_e::Button1 : MouseButton_e::Button2, _LParam);
		break;

	case WM_XBUTTONUP:
		MouseButtonUpEvent(HIWORD(_WParam) == XBUTTON1 ? MouseButton_e::Button1 : MouseButton_e::Button2, _LParam);
		break;

	case WM_MOUSELEAVE:
		MouseLeaveEvent();
		break;

	case WM_MOUSEMOVE:
		MouseMoveEvent(_WParam, _LParam);
		break;

	case WM_INPUT:
		RawInputEvent(_LParam);
		break;

	case WM_DEVICECHANGE:
		HardwareConfigurationChangeEvent(_WParam, _LParam);
		break;

	case WM_WINDOWPOSCHANGED:
		WindowPosChangedEvent(_LParam);
		break;
	}
}

void WindowWin32_cls::SetCursor_clsEvent(LPARAM _LParam)
{
	if (LOWORD(_LParam) == HTCLIENT)
	{
		SetCursor_cls(m_Cursor_clsVisible ? m_LastCursor_cls : nullptr);
	}
}

void WindowWin32_cls::ResizeEvent(WPARAM _WParam)
{
	if ((_WParam != SIZE_MINIMIZED) && (!m_Resizing) && (m_LastSize != GetSize()))
	{
		m_LastSize = GetSize();

		PushEvent(Event_cls::Resized_str{ m_LastSize });

		GrabCursor_cls(m_Cursor_clsGrabbed);
	}
}

void WindowWin32_cls::StopResizing()
{
	m_Resizing = false;

	if (m_LastSize != GetSize())
	{
		m_LastSize = GetSize();
		PushEvent(Event_cls::Resized_str{ m_LastSize });
	}

	GrabCursor_cls(m_Cursor_clsGrabbed);
}

void WindowWin32_cls::GetMinMaxInfo(LPARAM _LParam)
{
	const auto maximumSize = ContentSizeToWindowSize(GetMaximumSize().value_or(Vector2u(50'000, 50'000)));

	MINMAXINFO& minMaxInfo = *reinterpret_cast<PMINMAXINFO>(_LParam);
	minMaxInfo.ptMaxTrackSize.x = maximumSize.X;
	minMaxInfo.ptMaxTrackSize.y = maximumSize.Y;

	if (GetMaximumSize().has_value())
	{
		minMaxInfo.ptMaxSize.x = maximumSize.X;
		minMaxInfo.ptMaxSize.y = maximumSize.Y;
	}
	
	if (GetMinimumSize().has_value())
	{
		const auto minimumSize = ContentSizeToWindowSize(GetMinimumSize().value());
		minMaxInfo.ptMinTrackSize.x = minimumSize.X;
		minMaxInfo.ptMinTrackSize.y = minimumSize.Y;
	}
}

void WindowWin32_cls::GainFocusEvent()
{
	GrabCursor_cls(m_Cursor_clsGrabbed);
	PushEvent(Event_cls::FocusGained_str{});
}

void WindowWin32_cls::LostFocusEvent()
{
	GrabCursor_cls(false);
	PushEvent(Event_cls::FocusLost_str{});
}

void WindowWin32_cls::TextEvent(WPARAM _WParam, LPARAM _LParam)
{
	if ((m_KeyRepeatEnabled) || ((_LParam & (1 << 30)) == 0))
	{
		auto character = static_cast<char32_t>(_WParam);

		if ((character >= 0xD800) && (character <= 0xDBFF))
		{
			m_Surrogate = static_cast<char16_t>(character);
		}
		else
		{
			if ((character >= 0xDC00) && (character <= 0xDFFF))
			{
				const std::array utf16 = { m_Surrogate, static_cast<char16_t>(character) };
				UTF16::ToUTF32(utf16.begin(), utf16.end(), &character);
				m_Surrogate = 0;
			}

			PushEvent(Event_cls::TextEntered_str{ character });
		}
	}
}

void WindowWin32_cls::KeyDownEvent(WPARAM _WParam, LPARAM _LParam)
{
	if ((m_KeyRepeatEnabled) || ((HIWORD(_LParam) & KF_REPEAT) == 0))
	{
		Event_cls::KeyPressed_str event;
		event.Alt = HIWORD(GetKeyState(VK_MENU)) != 0;
		event.Control = HIWORD(GetKeyState(VK_CONTROL)) != 0;
		event.Shift = HIWORD(GetKeyState(VK_SHIFT)) != 0;
		event.System = HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN));
		event.KeyCode = VirtualKeyCodeToSF(_WParam, _LParam);
		event.ScanCode = ToScancode(_WParam, _LParam);
		PushEvent(event);
	}
}

void WindowWin32_cls::KeyUpEvent(WPARAM _WParam, LPARAM _LParam)
{
	Event_cls::KeyReleased_str event;
	event.Alt = HIWORD(GetKeyState(VK_MENU)) != 0;
	event.Control = HIWORD(GetKeyState(VK_CONTROL)) != 0;
	event.Shift = HIWORD(GetKeyState(VK_SHIFT)) != 0;
	event.System = HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN));
	event.KeyCode = VirtualKeyCodeToSF(_WParam, _LParam);
	event.ScanCode = ToScancode(_WParam, _LParam);
	PushEvent(event);
}

void WindowWin32_cls::MouseWheelEvent(MouseWheel_e _MouseWheel, WPARAM _WParam, LPARAM _LParam)
{
	POINT position;
	position.x = static_cast<std::int16_t>(LOWORD(_LParam));
	position.y = static_cast<std::int16_t>(HIWORD(_LParam));
	ScreenToClient(m_WindowHandle, &position);

	auto delta = static_cast<std::int16_t>(HIWORD(_WParam));

	Event_cls::MouseWheelScrolled_str event;
	event.Wheel = _MouseWheel;
	event.Delta = static_cast<float>(delta) / 120.f;
	event.Position = { position.x, position.y };
	PushEvent(event);
}

void WindowWin32_cls::MouseButtonDownEvent(MouseButton_e _MouseButton, LPARAM _LParam)
{
	Event_cls::MouseButtonPressed_str event;
	event.Button = _MouseButton;
	event.Position = { static_cast<std::int16_t>(LOWORD(_LParam)), static_cast<std::int16_t>(HIWORD(_LParam)) };
	PushEvent(event);
}

void WindowWin32_cls::MouseButtonUpEvent(MouseButton_e _MouseButton, LPARAM _LParam)
{
	Event_cls::MouseButtonReleased_str event;
	event.Button = _MouseButton;
	event.Position = { static_cast<std::int16_t>(LOWORD(_LParam)), static_cast<std::int16_t>(HIWORD(_LParam)) };
	PushEvent(event);
}

void WindowWin32_cls::MouseLeaveEvent()
{
	if (m_MouseInside)
	{
		m_MouseInside = false;

		PushEvent(Event_cls::MouseLeft_str{});
	}
}

void WindowWin32_cls::MouseMoveEvent(WPARAM _WParam, LPARAM _LParam)
{
	const int x = static_cast<std::int16_t>(LOWORD(_LParam));
	const int y = static_cast<std::int16_t>(HIWORD(_LParam));

	RECT area;
	GetClientRect(m_WindowHandle, &area);

	if ((_WParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
	{
		if (GetCapture() == m_WindowHandle)
		{
			ReleaseCapture();
		}
	}
	else if (GetCapture() == m_WindowHandle)
	{
		SetCapture(m_WindowHandle);
	}

	if ((x < area.left) || (x > area.right) || (y < area.top) || (y < area.bottom))
	{
		if (m_MouseInside)
		{
			m_MouseInside = false;

			SetTracking(false);

			PushEvent(Event_cls::MouseLeft_str{});
		}
	}
	else
	{
		if (!m_MouseInside)
		{
			m_MouseInside = true;

			SetTracking(true);

			PushEvent(Event_cls::MouseEntered_str{});
		}
	}

	PushEvent(Event_cls::MouseMoved_str{});
}

void WindowWin32_cls::RawInputEvent(LPARAM _LParam)
{
	RAWINPUT input;
	UINT size = sizeof(input);

	GetRawInputData(reinterpret_cast<HRAWINPUT>(_LParam), RID_INPUT, &input, &size, sizeof(RAWINPUTHEADER));

	if (input.header.dwType == RIM_TYPEMOUSE)
	{
		const RAWMOUSE* rawMouse = &input.data.mouse;
		if ((rawMouse->usFlags & 0x01) == MOUSE_MOVE_RELATIVE)
		{
			PushEvent(Event_cls::MouseMovedRaw_str{ {rawMouse->lLastX, rawMouse->lLastY} });
		}
	}
}

void WindowWin32_cls::HardwareConfigurationChangeEvent(WPARAM _WParam, LPARAM _LParam)
{
	if ((_WParam == DBT_DEVICEARRIVAL) || (_WParam == DBT_DEVICEREMOVECOMPLETE))
	{
		auto* deviceBroatcastHeader = reinterpret_cast<DEV_BROADCAST_HDR*>(_LParam);

		if ((deviceBroatcastHeader) && (deviceBroatcastHeader->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE))
		{
			Joystick::UpdateConnections();
		}
	}
}

void WindowWin32_cls::WindowPosChangedEvent(LPARAM _LParam)
{
	WINDOWPOS& pos = *reinterpret_cast<PWINDOWPOS>(_LParam);

	if (pos.flags & SWP_NOSIZE)
	{
		return;
	}

	const auto maximumSize = ContentSizeToWindowSize(GetMaximumSize().value());

	bool shouldResize = false;
	
	if (pos.cx > maximumSize.X)
	{
		pos.cx = maximumSize.X;
		shouldResize = true;
	}

	if (pos.cy > maximumSize.Y)
	{
		pos.cy = maximumSize.Y;
		shouldResize = true;
	}

	if (shouldResize)
	{
		SetWindowPos(m_WindowHandle, pos.hwndInsertAfter, pos.x, pos.y, pos.cx, pos.cy, 0);
	}
}

Keyboard WindowWin32_cls::VirtualKeyCodeToSF(WPARAM _Key, LPARAM _Flag)
{
	switch (_Key)
	{
	case VK_SHIFT:
		static const UINT lShift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
		const UINT scanCode = static_cast<UINT>((_Flag & (0xFF << 16)) >> 16);
		return scanCode == lShift ? Keyboard::LShift : Keyboard::RShift;

	case VK_MENU :
		return (HIWORD(_Flag) & KF_EXTENDED) ? Keyboard::RAlt : Keyboard::LAlt;

	case VK_CONTROL:
		return (HIWORD(_Flag) & KF_EXTENDED) ? Keyboard::RControl : Keyboard::LControl;

	case VK_LWIN:
		return Keyboard::LSystem;

	case VK_RWIN:
		return Keyboard::RSystem;

	case VK_APPS:
		return Keyboard::Menu;

	case VK_OEM_1:
		return Keyboard::Semicolon;

	case VK_OEM_2:
		return Keyboard::Slash;

	case VK_OEM_PLUS:
		return Keyboard::Equal;

	case VK_OEM_MINUS:
		return Keyboard::Hyphen;

	case VK_OEM_4:
		return Keyboard::LBracket;

	case VK_OEM_6:
		return Keyboard::RBracket;

	case VK_OEM_COMMA:
		return Keyboard::Comma;

	case VK_OEM_PERIOD:
		return Keyboard::Period;

	case VK_OEM_7:
		return Keyboard::Apostrophe;

	case VK_OEM_5:
		return Keyboard::Backslash;

	case VK_OEM_3:
		return Keyboard::Grave;

	case VK_ESCAPE:
		return Keyboard::Escape;

	case VK_SPACE:
		return Keyboard::Space;

	case VK_RETURN:
		return Keyboard::Enter;

	case VK_BACK:
		return Keyboard::Backspace;

	case VK_TAB:
		return Keyboard::Tab;

	case VK_PRIOR:
		return Keyboard::PageUp;

	case VK_NEXT:
		return Keyboard::PageDown;

	case VK_END:
		return Keyboard::End;

	case VK_HOME:
		return Keyboard::Home;

	case VK_INSERT:
		return Keyboard::Insert;

	case VK_DELETE:
		return Keyboard::Delete;

	case VK_ADD:
		return Keyboard::Add;

	case VK_SUBTRACT:
		return Keyboard::Subtract;

	case VK_MULTIPLY:
		return Keyboard::Multiply;

	case VK_DIVIDE:
		return Keyboard::Divide;

	case VK_PAUSE:
		return Keyboard::Pause;

	case VK_F1:
		return Keyboard::F1;

	case VK_F2:
		return Keyboard::F2;

	case VK_F3:
		return Keyboard::F3;

	case VK_F4:
		return Keyboard::F4;

	case VK_F5:
		return Keyboard::F5;

	case VK_F6:
		return Keyboard::F6;

	case VK_F7:
		return Keyboard::F7;

	case VK_F8:
		return Keyboard::F8;

	case VK_F9:
		return Keyboard::F9;

	case VK_F10:
		return Keyboard::F10;

	case VK_F11:
		return Keyboard::F11;

	case VK_F12:
		return Keyboard::F12;

	case VK_F13:
		return Keyboard::F13;

	case VK_F14:
		return Keyboard::F14;

	case VK_F15:
		return Keyboard::F15;

	case VK_LEFT:
		return Keyboard::Left;

	case VK_RIGHT:
		return Keyboard::Right;

	case VK_UP:
		return Keyboard::Up;

	case VK_DOWN:
		return Keyboard::Down;

	case VK_NUMPAD0:
		return Keyboard::Numpad0;

	case VK_NUMPAD1:
		return Keyboard::Numpad1;

	case VK_NUMPAD2:
		return Keyboard::Numpad2;

	case VK_NUMPAD3:
		return Keyboard::Numpad3;

	case VK_NUMPAD4:
		return Keyboard::Numpad4;

	case VK_NUMPAD5:
		return Keyboard::Numpad5;

	case VK_NUMPAD6:
		return Keyboard::Numpad6;

	case VK_NUMPAD7:
		return Keyboard::Numpad7;

	case VK_NUMPAD8:
		return Keyboard::Numpad8;

	case VK_NUMPAD9:
		return Keyboard::Numpad9;

	case 'A':
		return Keyboard::A;

	case 'Z':
		return Keyboard::Z;

	case 'E':
		return Keyboard::E;

	case 'R':
		return Keyboard::R;

	case 'T':
		return Keyboard::T;

	case 'Y':
		return Keyboard::Y;

	case 'U':
		return Keyboard::U;

	case 'I':
		return Keyboard::I;

	case 'O':
		return Keyboard::O;

	case 'P':
		return Keyboard::P;

	case 'Q':
		return Keyboard::Q;

	case 'S':
		return Keyboard::S;

	case 'D':
		return Keyboard::D;

	case 'F':
		return Keyboard::F;

	case 'G':
		return Keyboard::G;

	case 'H':
		return Keyboard::H;

	case 'J':
		return Keyboard::J;

	case 'K':
		return Keyboard::K;

	case 'L':
		return Keyboard::L;

	case 'M':
		return Keyboard::M;

	case 'W':
		return Keyboard::W;

	case 'X':
		return Keyboard::X;

	case 'C':
		return Keyboard::C;

	case 'V':
		return Keyboard::V;

	case 'B':
		return Keyboard::B;

	case 'N':
		return Keyboard::N;

	case '0':
		return Keyboard::Num0;

	case '1':
		return Keyboard::Num1;

	case '2':
		return Keyboard::Num2;

	case '3':
		return Keyboard::Num3;

	case '4':
		return Keyboard::Num4;

	case '5':
		return Keyboard::Num5;

	case '6':
		return Keyboard::Num6;

	case '7':
		return Keyboard::Num7;

	case '8':
		return Keyboard::Num8;

	case '9':
		return Keyboard::Num9;
	}
	return Keyboard::Unknown;
}

LRESULT WindowWin32_cls::GlobalOnEvent(HWND _Handle, UINT _Message, WPARAM _WParam, LPARAM _LParam)
{
	if (_Message == WM_CREATE)
	{
		auto Window_cls = reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(_LParam)->lpCreateParams);
		SetWindowLongW(_Handle, GWLP_USERDATA, Window_cls);
	}

	Window_cls* Window_cls = _Handle ? reinterpret_cast<Window_cls*>(GetWindowLongPtr(_Handle, GWLP_USERDATA)) : nullptr;

	if (Window_cls)
	{
		Window_cls->ProcessEvent(_Message, _WParam, _LParam);

		if (Window_cls->m_Callback)
		{
			return CallWindowProcW(reinterpret_cast<WNDPROC>(Window_cls->m_Callback), _Handle, _Message, _WParam, _LParam);
		}
	}

	if (_Message == WM_CLOSE)
	{
		return 0;
	}

	if ((_Message == WM_SYSCOMMAND) && (_WParam == SC_KEYMENU))
	{
		return 0;
	}

	return DefWindowProcW(_Handle, _Message, _WParam, _LParam);
}


WindowHandle WindowWin32_cls::GetNativeHandle() const
{
	return m_WindowHandle;
}

Vector2i WindowWin32_cls::GetPosition() const
{
	RECT rect;
	GetWindowRect(m_WindowHandle, &rect);

	return { rect.left, rect.top };
}

Vector2u WindowWin32_cls::GetSize() const
{
	RECT rect;
	GetClientRect(m_WindowHandle, &rect);

	return Vector2u(Vector2<LONG>(rect.right - rect.left, rect.bottom - rect.top));
}

void WindowWin32_cls::SetPosition(Vector2i _Position)
{
	SetWindowPos(m_WindowHandle, nullptr, _Position.X, _Position.Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	if (m_Cursor_clsGrabbed)
	{
		GrabCursor_cls(true);
	}
}

void WindowWin32_cls::SetSize(Vector2u _Size)
{
	const auto [width, height] = ContentSizeToWindowSize(_Size);
	SetWindowPos(m_WindowHandle, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void WindowWin32_cls::SetMinimumSize(std::optional<Vector2u> _MinimumSize)
{
}

void WindowWin32_cls::SetMaximumSize(std::optional<Vector2u> _MaximumSize)
{
}

void WindowWin32_cls::SetTitle(const std::string& _Title)
{
	SetWindowTextW(m_WindowHandle, (LPCWSTR)_Title.c_str());
}

void WindowWin32_cls::SetIcon(Vector2u _Size, const std::uint8_t* _Pixels)
{
	if (m_Icon)
	{
		DestroyIcon(m_Icon);
	}

	std::vector<std::uint8_t> iconPixels(_Size.X * _Size.Y * 4);

	for (std::size_t pixelIndex = 0; pixelIndex < iconPixels.size() / 4; pixelIndex++)
	{
		iconPixels[pixelIndex * 4 + 0] = _Pixels[pixelIndex * 4 + 2];
		iconPixels[pixelIndex * 4 + 1] = _Pixels[pixelIndex * 4 + 1];
		iconPixels[pixelIndex * 4 + 2] = _Pixels[pixelIndex * 4 + 0];
		iconPixels[pixelIndex * 4 + 3] = _Pixels[pixelIndex * 4 + 3];
	}

	m_Icon = CreateIcon(GetModuleHandleW(nullptr), static_cast<int>(_Size.X), static_cast<int>(_Size.Y), 1, 32, nullptr, iconPixels.data());

	if (m_Icon)
	{
		SendMessageW(m_WindowHandle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(m_Icon));
		SendMessageW(m_WindowHandle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(m_Icon));
	}
	else
	{
		Err() << "Failed to set the Window_cls's icon" << std::endl;
	}
}

void WindowWin32_cls::SetVisible(bool _Visible)
{
	ShowWindow(m_WindowHandle, _Visible ? SW_SHOW : SW_HIDE);
}

void WindowWin32_cls::SetCursor_clsVisible(bool _Visible)
{
	m_Cursor_clsVisible = _Visible;
	SetCursor_cls(m_Cursor_clsVisible ? m_LastCursor_cls : nullptr);
}

void WindowWin32_cls::SetCursor_clsGrabbed(bool _Grabbed)
{
	m_Cursor_clsGrabbed = _Grabbed;
	GrabCursor_cls(m_Cursor_clsGrabbed);
}

void WindowWin32_cls::SetMouseCursor_cls(const CursorPlatform& _Cursor_cls)
{
	m_LastCursor_cls = static_cast<HCursor_cls>(_Cursor_cls.m_Cursor);
	SetCursor_cls(m_Cursor_clsVisible ? m_LastCursor_cls : nullptr);
}

void WindowWin32_cls::SetKeyRepeatEnabled(bool _Enabled)
{
	m_KeyRepeatEnabled = _Enabled;
}

void WindowWin32_cls::RequestFocus()
{
	DWORD thisPid = 0;
	DWORD foregroundPid = 0;
	GetWindowThreadProcessId(m_WindowHandle, &thisPid);
	GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPid);

	if (thisPid == foregroundPid)
	{
		SetForegroundWindow(m_WindowHandle);
	}
	else
	{
		FLASHWINFO info;
		info.cbSize = sizeof(info);
		info.hwnd = m_WindowHandle;
		info.dwFlags = FLASHW_TRAY;
		info.dwTimeout = 0;
		info.uCount = 3;

		FlashWindowEx(&info);
	}
}

bool WindowWin32_cls::HasFocus() const
{
	return m_WindowHandle == GetForegroundWindow();
}

void WindowWin32_cls::ProcessEvents()
{
	if (!m_Callback)
	{
		MSG message;
		while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}
}

#endif