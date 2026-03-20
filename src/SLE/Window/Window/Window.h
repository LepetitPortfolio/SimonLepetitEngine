#pragma once
#include "../../Platform.h"
#include "Cursor.h"
#include "Event.h"
#include "WindowEnums.h"
#include "WindowConfig.h"
#include "WindowSettings.h"

#include "../../Common/EnumArray.h"
#include "../../Common/Time.h"
#include "../../Common/Vector.h"

#include <string>
#include <cstdint>
#include <array>
#include <memory>
#include <optional>
#include <queue>

struct WindowJoystickStates;

class Window
{

public:
	static std::unique_ptr<Window> CreateNewWindow(WindowConfig _WindowConfig, const std::string& _Title, std::uint32_t _Style, WindowState _State, const WindowSettings& _WindowSettings);

	static std::unique_ptr<Window> CreateNewWindow(WindowHandle _Handle);
	
	virtual ~Window();

	Window(const Window&) = delete;

	Window& operator=(const Window&) = delete;

	void SetJoystickThreshold(float _Threshold);

	std::optional<Event> WaitEvent(Time _TimeOut);

	std::optional<Event> PollEvent();

	virtual WindowHandle GetNativeHandle() const = 0;

	virtual Vector2i GetPosition() const = 0;
	virtual void SetPosition(Vector2i _Position) = 0;

	virtual Vector2u GetSize() const = 0;
	virtual void SetSize(Vector2u _Size) = 0;
	std::optional<Vector2u> GetMinimumSize() const;
	virtual void SetMinimumSize(std::optional<Vector2u> _MinimumSize) = 0;
	std::optional<Vector2u> GetMaximumSize() const;
	virtual void SetMaximumSize(std::optional<Vector2u> _MaximumSize) = 0;

	virtual void SetTitle(const std::string& _Title) = 0;

	virtual void SetIcon(Vector2u _Size, const std::uint8_t* _Pixels) = 0;

	virtual void SetVisible(bool _Visible) = 0;

	virtual void SetCursorVisible(bool _Visible) = 0;

	virtual void SetCursorGrabbed(bool _Grabbed) = 0;

	virtual void SetMouseCursor(const CursorData& _Cursor) = 0;

	virtual void SetKeyRepeatEnabled(bool _Enabled) = 0;

	virtual void RequestFocus() = 0;

	virtual  bool HasFocus() const = 0;

	virtual void ProcessEvents() = 0;

protected:

	static Window* m_WindowInstance;

	std::queue<Event> m_Events;
	std::unique_ptr<WindowJoystickStates> m_JoystickStates;
	EnumArray<SensorType, Vector3f, SensorTypeCount> m_SensorValue;
	float m_JoystickThreshold{ 0.1f }; 
	std::array<EnumArray<JoystickAxis, float, JoystickAxisCount>, JoystickCount> m_PreviousAxes{}; 
	std::optional<Vector2u> m_MinimumSize;
	std::optional<Vector2u> m_MaximumSize;

	Window();

	virtual void Cleanup() = 0;

	void PushEvent(const Event& _Event);

	virtual void ProcessEvents() = 0;

	std::optional<Event> PopEvent();

	void ProcessJoystickEvents();

	void ProcessSensorEvents();

	void PopulateEventQueue();

};

#if PLATFORM_WINDOWS
#include "Win/WindowWin32.h"
using WindowType = WindowWin32;
#elif PLATFORM_LINUX
#include "Unix/WindowUnix.h"
using WindowType = WindowUnix;
#endif