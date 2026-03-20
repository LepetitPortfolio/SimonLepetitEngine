#pragma once
#include "../../../Platform.h"

#if PLATFORM_LINUX

#include "../Window.h"

#include "../Cursor.h"
#include "Display.h"
#include "../Event.h"
#include "../WindowEnums.h"
#include "../WindowConfig.h"
#include "../WindowSettings.h"

#include "../../../Common/EnumArray.h"
#include "../../../Common/Time.h"
#include "../../../Common/String.h"
#include "../../../Common/Vector.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <queue>
#include <string>


class WindowUnix : public Window
{

public:
	WindowUnix(WindowHandle _Handle);

	WindowUnix(WindowConfig _WindowConfig, const String& _Title, std::uint32_t _Style, WindowState _State, const WindowSettings& _WindowSettings);

	~WindowUnix() override;


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

	virtual  bool HasFocus() const override;

	virtual void ProcessEvents() override;

protected:


	Window(WindowConfig _WindowConfig, const std::string& _Title, std::uint32_t _Style, WindowState _State, const WindowSettings& _WindowSettings);

	Window m_Window{};
	std::shared_ptr<Display> m_Display;
	int                      m_Screen;
	std::shared_ptr<_XIM>    m_InputMethod;
	XIC m_InputContext{};
	bool m_IsExternal{};
	RRMode m_OldVideoMode{};
	RRCrtc m_OldRRCrtc{};
	Cursor m_HiddenCursor{}; 
	Cursor m_LastCursor{ None };
	bool m_keyRepeat{ true }; 
	Vector2i m_PreviousSize{ -1, -1 }; 
	bool m_UseSizeHints{};  
	bool m_Fullscreen{};    
	bool m_CursorGrabbed{}; 
	bool m_WindowMapped{};  
	Pixmap m_IconPixmap{};    
	Pixmap m_IconMaskPixmap{};
	Time m_LastInputTime{};

	WindowUnix* m_FullscreenWindow = nullptr;
	std::vector<WindowUnix*> m_AllWindows;
	std::bitset<256> m_IsKeyFiltered;
	std::recursive_mutex m_AllWindowsMutex;
	std::tring m_WindowManagerName;

	unsigned long m_EventMask = FocusChangeMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask |
								EnterWindowMask | LeaveWindowMask | VisibilityChangeMask | PropertyChangeMask;

	unsigned int m_MaxTrialsCount = 5;

	virtual void Cleanup() override;

	virtual void ProcessEvents() override;

	bool EwmhSupported();

	void SetProtocols();

	std::filesystem::path FindExecutableName();

	void SetTitle(const String& _Title);

	void Initialize();

	bool InitRawMouse(Display* _Display);

	void SetWindowConfig(const WindowConfig& _WindowConfig);
	
	void ResetWindowConfig();

	bool CheckXRandR();

	RROutput GetOutputPrimary(Window& _RootWindow, XRRScreenResources* _ScreenResources);

	void SwitchToFullscreen();

	bool ProcessEvent(XEvent& _WindowEvent);

};

#endif