#pragma once

#include "../../../Platform.h"

#if PLATFORM_WINDOWS

#include "../Window.h"

#include "../Cursor.h"
#include "../Event.h"
#include "../WindowEnums.h"
#include "../WindowConfig.h"
#include "../WindowSettings.h"

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

class WindowWin32 : public Window
{

public:

	WindowWin32(WindowHandle _Handle);

	WindowWin32(WindowConfig _WindowConfig, const std::string& _Title, std::uint32_t _Style, WindowState _State, const WindowSettings& _WindowSettings);

	~WindowWin32() override;

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

	virtual void SetCursorVisible(bool _Visible) override;

	virtual void SetCursorGrabbed(bool _Grabbed) override;

	virtual void SetMouseCursor(const CursorData& _Cursor) override;

	virtual void SetKeyRepeatEnabled(bool _Enabled) override;

	virtual void RequestFocus() override;

	virtual bool HasFocus() const override;


protected:

	HWND m_WindowHandle{};
	LONG_PTR m_Callback{};
	bool m_CursorVisible{ true };
	HCURSOR m_LastCursor{ LoadCursor(nullptr, IDC_ARROW) };
	HICON m_Icon{};
	bool m_KeyRepeatEnabled{ true };
	Vector2u m_LastSize;
	bool m_Resizing{};
	char16_t m_Surrogate{};
	bool m_MouseInside{};
	bool m_FullScreen{};
	bool m_CursorGrabbed{};

	unsigned int m_WindowCount = 0;
	unsigned int m_HandleCount = 0;
	const wchar_t* m_ClassName = L"SLE_Window";
	WindowWin32* m_FullScreenWindow = nullptr;

	GUID m_GuidDevinterfaceHid = { 0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30} };

	void SetProcessDpiAware();

	void InitRawMouse();

	void RegisterWindowClass();

	void SwitchToFullscreen(const WindowConfig& _WindowConfig);

	void SetTracking(bool _Track);

	void GrabCursor(bool _Grabbed);

	Vector2i ContentSizeToWindowSize(Vector2u _Size);

	ScanCode ToScancode(WPARAM _WParam, LPARAM _LParam);

	void ProcessEvent(UINT _Message, WPARAM _WParam, LPARAM _LParam);

	void SetCursorEvent(LPARAM _LParam);

	void ResizeEvent(WPARAM _WParam);

	void StopResizing();

	void GetMinMaxInfo(LPARAM _LParam);

	void GainFocusEvent();

	void LostFocusEvent();

	void TextEvent(WPARAM _WParam, LPARAM _LParam);

	void KeyDownEvent(WPARAM _WParam, LPARAM _LParam);

	void KeyUpEvent(WPARAM _WParam, LPARAM _LParam);

	void MouseWheelEvent(MouseWheel _MouseWheel, WPARAM _WParam, LPARAM _LParam);

	void MouseButtonDownEvent(MouseButton _MouseButton, LPARAM _LParam);

	void MouseButtonUpEvent(MouseButton _MouseButton, LPARAM _LParam);

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