#pragma once
#include "../../DataConfig.h"
#include "Cursor_cls.h"
#include "Event_cls.h"

#include "WindowEnums.h"
#include "WindowConfig_cls.h"
#include "WindowSettings_str.h"

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

class WindowPlatform_cls
{

public:
	static std::unique_ptr<WindowPlatform_cls> CreateNewWindow(WindowConfig _WindowConfig, const String& _Title, std::uint32_t _Style, WindowState_e _State, const WindowSettings_str& _WindowSettings);

	static std::unique_ptr<WindowPlatform_cls> CreateNewWindow(WindowHandle _Handle);
	
	virtual ~WindowPlatform_cls();

	WindowPlatform_cls(const WindowPlatform_cls&) = delete;

	WindowPlatform_cls& operator=(const WindowPlatform_cls&) = delete;

	void SetJoystickThreshold(float _Threshold);

	std::optional<Event_cls> WaitEvent(Time _TimeOut);

	std::optional<Event_cls> PollEvent();

	virtual WindowHandle GetNativeHandle() const = 0;

	virtual Vector2i GetPosition() const = 0;
	virtual void SetPosition(Vector2i _Position) = 0;

	virtual Vector2u GetSize() const = 0;
	virtual void SetSize(Vector2u _Size) = 0;
	std::optional<Vector2u> GetMinimumSize() const;
	virtual void SetMinimumSize(std::optional<Vector2u> _MinimumSize) = 0;
	std::optional<Vector2u> GetMaximumSize() const;
	virtual void SetMaximumSize(std::optional<Vector2u> _MaximumSize) = 0;

	virtual void SetTitle(const String& _Title) = 0;

	virtual void SetIcon(Vector2u _Size, const std::uint8_t* _Pixels) = 0;

	virtual void SetVisible(bool _Visible) = 0;

	virtual void SetCursor_clsVisible(bool _Visible) = 0;

	virtual void SetCursor_clsGrabbed(bool _Grabbed) = 0;

	virtual void SetMouseCursor_cls(const CursorPlatform& _Cursor_cls) = 0;

	virtual void SetKeyRepeatEnabled(bool _Enabled) = 0;

	virtual void RequestFocus() = 0;

	virtual  bool HasFocus() const = 0;

    bool CreateVulkanSurface(const VkInstance& _Instance, VkSurfaceKHR _Surface, const VkAllocationCallbacks* _Allocator) const;

protected:

	static WindowPlatform_cls* m_WindowInstance;

	std::queue<Event_cls> m_Events;
	std::unique_ptr<WindowJoystickStates> m_JoystickStates;
	EnumArray<SensorType_e, Vector3f, SensorTypeCount> m_SensorValue;
	float m_JoystickThreshold{ 0.1f }; 
	std::array<EnumArray<JoystickAxis_e, float, JoystickAxisCount>, JoystickCount> m_PreviousAxes{}; 
	std::optional<Vector2u> m_MinimumSize;
	std::optional<Vector2u> m_MaximumSize;

	WindowPlatform_cls();

	virtual void Cleanup() = 0;

	void PushEvent(const Event_cls& _Event);

	virtual void ProcessEvents() = 0;

	std::optional<Event_cls> PopEvent();

	void ProcessJoystickEvents();

	void ProcessSensorEvents();

	void PopulateEventQueue();

	void SetWindowSizeContraints();

};

#if PLATFORM_WINDOWS
#include "Win/WindowWin32_cls.h"
using WindowType = WindowWin32_cls;
#elif PLATFORM_LINUX
#include "Unix/WindowUnix_cls.h"
using WindowType = WindowUnix_cls;
#endif