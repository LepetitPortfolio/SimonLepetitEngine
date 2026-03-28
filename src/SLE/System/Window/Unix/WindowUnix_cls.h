#pragma once
#include "../../../PlatformConfig.h"

#if PLATFORM_LINUX

#include "../WindowPlatform_cls.h"

#include "../Cursor_cls.h"
#include "Display.h"
#include "../Event_cls.h"
#include "../WindowEnums.h"
#include "../WindowConfig_cls.h"
#include "../WindowSettings_str.h"

#include "../../../Common/EnumArray.h"
#include "../../../Common/Time.h"
#include "../../../Common/String.h"
#include "../../../Common/Vector.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <queue>
#include <mutex>
#include <string>


class WindowUnix_cls : public WindowPlatform_cls
{

public:
	WindowUnix_cls(WindowHandle _Handle);

	WindowUnix_cls(WindowConfig _WindowConfig, const String& _Title, std::uint32_t _Style, WindowState_e _State, const WindowSettings_str& _WindowSettings);

	~WindowUnix_cls() override;


	virtual WindowHandle GetNativeHandle() const override;

	virtual Vector2i GetPosition() const override;
	virtual void SetPosition(Vector2i _Position) override;

	virtual Vector2u GetSize() const override;
	virtual void SetSize(Vector2u _Size) override;

	virtual void SetMinimumSize(std::optional<Vector2u> _MinimumSize) override;

	virtual void SetMaximumSize(std::optional<Vector2u> _MaximumSize) override;

	virtual void SetTitle(const String& _Title) override;

	virtual void SetIcon(Vector2u _Size, const std::uint8_t* _Pixels) override;

	virtual void SetVisible(bool _Visible) override;

	virtual void SetCursorVisible(bool _Visible) override;

	virtual void SetCursorGrabbed(bool _Grabbed) override;

	virtual void SetMouseCursor(const CursorPlatform& _Cursor) override;

	virtual void SetKeyRepeatEnabled(bool _Enabled) override;

	virtual void RequestFocus() override;

	virtual  bool HasFocus() const override;

	virtual void ProcessEvents() override;

protected:
	
	Window_cls m_Window{};
	std::shared_ptr<Display> m_Display;
	int m_Screen;
	std::shared_ptr<_XIM> m_InputMethod;
	XIC m_InputContext{};
	bool m_IsExternal{};
	RRMode m_OldVideoMode{};
	RRCrtc m_OldRRCrtc{};
	Cursor m_HiddenCursor{};
	Cursor m_LastCursor{None};
	bool m_keyRepeat{ true }; 
	Vector2i m_PreviousSize{ -1, -1 }; 
	bool m_UseSizeHints{};  
	bool m_Fullscreen{};    
	bool m_CursorGrabbed{}; 
	bool m_WindowMapped{};  
	Pixmap m_IconPixmap{};    
	Pixmap m_IconMaskPixmap{};
	Time m_LastInputTime{};

	WindowUnix_cls* m_FullscreenWindow = nullptr;
	std::vector<WindowUnix_cls*> m_AllWindows;
	std::bitset<256> m_IsKeyFiltered;
	std::recursive_mutex m_AllWindowsMutex;
	String m_WindowManagerName;

	unsigned long m_EventMask = FocusChangeMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask |
								EnterWindowMask | LeaveWindowMask | VisibilityChangeMask | PropertyChangeMask;

	unsigned int m_MaxTrialsCount = 5;

	virtual void Cleanup() override;

	virtual void ProcessEvents() override;

	void GrabFocus();

	bool EwmhSupported();

	void SetProtocols();

	void UpdateLastInputTime(Time _Time);

	std::filesystem::path FindExecutableName();

	void SetTitle(const String& _Title);

	void Initialize();

	void CreateHiddenCursor();

	bool InitRawMouse(Display* _Display);

	void SetWindowConfig(const WindowConfig& _WindowConfig);
	
	void ResetWindowConfig();

	bool CheckXRandR();

	RROutput GetOutputPrimary(Window_cls& _RootWindow, XRRScreenResources* _ScreenResources);

	void SwitchToFullscreen();

	bool ProcessEvent(XEvent& _WindowEvent);

	void GainFocusEvent(XEvent& _WindowEvent);

	void LostFocusEvent(XEvent& _WindowEvent);

	void ResizeEvent(XEvent& _WindowEvent);

	void CloseEvent(XEvent& _WindowEvent);

	void KeyDownEvent(XEvent& _WindowEvent);

	void KeyUpEvent(XEvent& _WindowEvent);

	void MouseButtonPressed_str(XEvent& _WindowEvent);

	void MouseButtonRelease(XEvent& _WindowEvent);

	void MouseMoved_str(XEvent& _WindowEvent);

	void MouseEntered_str(XEvent& _WindowEvent);

	void MouseLeft_str(XEvent _WindowEvent);

	void KeyboardMappingChanged(XEvent _WindowEvent);

	void WindowInmapped(XEvent _WindowEvent);

	void WindowVisibilityChange(XEvent _WindowEvent);

	void WindowPropertyChange(XEvent _WindowEvent);

	void RawInput(XEvent _WindowEvent);


	bool IsWMAbsolutePositionGood();

	bool GetEWMHFrameExtents(Display* _Display, Window_cls _Window, long& _XFrameExtent, long& _YFrameExtent);

	Window_cls GetParentWindow(Display* _Display, Window_cls _Window);

	Vector2i GetPrimaryMonitorPosition();

	void SetWindowSizeContraints() const;
};

#endif