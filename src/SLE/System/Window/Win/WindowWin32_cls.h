#pragma once

#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "../WindowPlatform_cls.h"

#include "../Cursor_cls.h"
#include "../Event_cls.h"
#include "../WindowEnums.h"
#include "../WindowConfig_cls.h"
#include "../WindowSettings_str.h"

#include "../../../Common/EnumArray.h"
#include "../../../Common/Time.h"
#include "../../../Common/Vector.h"

#include <string>
#include <cstdint>
#include <array>
#include <memory>
#include <optional>
#include <queue>


struct WindowJoystickStates;

class WindowWin32_cls : public WindowPlatform_cls
{

public:

	WindowWin32_cls(WindowHandle _Handle);

	WindowWin32_cls(WindowConfig _WindowConfig, const std::string& _Title, std::uint32_t _Style, WindowState_e _State, const WindowSettings_str& _WindowSettings);

	~WindowWin32_cls() override;

	virtual WindowHandle GetNativeHandle() const override;

	virtual Vector2i GetPosition() const override;
	virtual void SetPosition(Vector2i _Position) override;

	virtual Vector2u GetSize() const override;
	virtual void SetSize(Vector2u _Size) override;

	virtual void SetMinimumSize(std::optional<Vector2u> _MinimumSize) override;

	virtual void SetMaximumSize(std::optional<Vector2u> _MaximumSize) override;

	virtual void SetTitle(const std::string& _Title) override;

	virtual void SetIcon(Vector2u _Size, const std::uint8_t* _Pixels) override;

	virtual void SetVisible(bool _Visible) override;

	virtual void SetCursor_clsVisible(bool _Visible) override;

	virtual void SetCursor_clsGrabbed(bool _Grabbed) override;

	virtual void SetMouseCursor_cls(const CursorPlatform& _Cursor_cls) override;

	virtual void SetKeyRepeatEnabled(bool _Enabled) override;

	virtual void RequestFocus() override;

	virtual bool HasFocus() const override;


protected:

	HWND m_WindowHandle{};
	LONG_PTR m_Callback{};
	bool m_Cursor_clsVisible{ true };
	HCursor_cls m_LastCursor_cls{ LoadCursor_cls(nullptr, IDC_ARROW) };
	HICON m_Icon{};
	bool m_KeyRepeatEnabled{ true };
	Vector2u m_LastSize;
	bool m_Resizing{};
	char16_t m_Surrogate{};
	bool m_MouseInside{};
	bool m_FullScreen{};
	bool m_Cursor_clsGrabbed{};

	unsigned int m_WindowCount = 0;
	unsigned int m_HandleCount = 0;
	const wchar_t* m_ClassName = L"SLE_Window";
	WindowWin32_cls* m_FullScreenWindow = nullptr;

	GUID m_GuidDevinterfaceHid = { 0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30} };

	void SetProcessDpiAware();

	void InitRawMouse();

	void RegisterWindowClass();

	void SwitchToFullscreen(const WindowConfig& _WindowConfig);

	void SetTracking(bool _Track);

	void GrabCursor_cls(bool _Grabbed);

	Vector2i ContentSizeToWindowSize(Vector2u _Size);

	ScanCode ToScancode(WPARAM _WParam, LPARAM _LParam);

	void ProcessEvent(UINT _Message, WPARAM _WParam, LPARAM _LParam);

	void SetCursor_clsEvent(LPARAM _LParam);

	void ResizeEvent(WPARAM _WParam);

	void StopResizing();

	void GetMinMaxInfo(LPARAM _LParam);

	void GainFocusEvent();

	void LostFocusEvent();

	void TextEvent(WPARAM _WParam, LPARAM _LParam);

	void KeyDownEvent(WPARAM _WParam, LPARAM _LParam);

	void KeyUpEvent(WPARAM _WParam, LPARAM _LParam);

	void MouseWheelEvent(MouseWheel_e _MouseWheel, WPARAM _WParam, LPARAM _LParam);

	void MouseButtonDownEvent(MouseButton_e _MouseButton, LPARAM _LParam);

	void MouseButtonUpEvent(MouseButton_e _MouseButton, LPARAM _LParam);

	void MouseLeaveEvent();

	void MouseMoveEvent(WPARAM _WParam, LPARAM _LParam);

	void RawInputEvent(LPARAM _LParam);

	void HardwareConfigurationChangeEvent(WPARAM _WParam, LPARAM _LParam);

	void WindowPosChangedEvent(LPARAM _LParam);

	Keyboard VirtualKeyCodeToSF(WPARAM _Key, LPARAM _Flag);

	LRESULT CALLBACK GlobalOnEvent(HWND _Handle, UINT _Message, WPARAM _WParam, LPARAM _LParam);

	virtual void Cleanup() override;

	virtual void ProcessEvents() override;
};

#endif