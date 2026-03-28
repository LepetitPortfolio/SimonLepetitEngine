#include "WindowPlatform_cls.h"

#include "../../Common/Error.h"
#include "../Inputs/JoystickManager_cls.h"
#include "../Inputs/SensorManager_cls.h"

struct WindowJoystickStates
{
	std::array<JoystickState, JoystickCount> States{};
};

WindowPlatform_cls* WindowPlatform_cls::m_WindowInstance = nullptr;

WindowPlatform_cls::WindowPlatform_cls() : m_JoystickStates(std::make_unique<WindowJoystickStates>())
{
	JoystickManager_cls::GetInstance().Update();
	
	for (unsigned int joystickIndex = 0; joystickIndex < JoystickCount; joystickIndex++)
	{
		m_JoystickStates->States[joystickIndex] = JoystickManager_cls::GetInstance().GetState(joystickIndex);
		m_PreviousAxes[joystickIndex].fill(0.f);
	}

	for (Vector3f& sensorValue : m_SensorValue)
	{
		sensorValue = Vector3f();
	}
}

WindowPlatform_cls::~WindowPlatform_cls() 
{
	if (m_WindowInstance == this)
	{
		m_WindowInstance = nullptr;
	}
}

std::unique_ptr<WindowPlatform_cls> WindowPlatform_cls::CreateNewWindow(WindowConfig _WindowConfig, const String& _Title, std::uint32_t _Style, WindowState_e _State, const WindowSettings_str& _WindowSettings)
{
	if (_State == WindowState_e::Fullscreen)
	{
		if (m_WindowInstance != nullptr)
		{
			Err() << "A Window_cls already exists, only one Window_cls can be created at a time." << std::endl;
			_State = WindowState_e::Windowed;
		}
		else if (!_WindowConfig.IsValid())
		{
			Err() << "The requested video mode is not valid, Window_cls creation failed." << std::endl;
			assert(!WindowConfig::GetFullscreenModes().empty() && "No Window_cls config available");
			_WindowConfig = WindowConfig::GetFullscreenModes()[0];

			Err() << "WindowConfig { Size : { " << _WindowConfig.m_Size.X << ", " << _WindowConfig.m_Size.Y << " }, bitsPerPixel: " << _WindowConfig.m_BitsPerPixel << " }" << std::endl;
		}

		if ((_Style & static_cast<std::uint32_t>(WindowStyle_e::Close)) || (_Style & static_cast<std::uint32_t>(WindowStyle_e::Resize)))
		{
			_Style |= static_cast<std::uint32_t>(WindowStyle_e::Titlebar);
		}

		auto Window_cls = std::make_unique<WindowType>(_WindowConfig, _Title, _Style, _State, _WindowSettings);

		if(_State == WindowState_e::Fullscreen)
		{
			m_WindowInstance = Window_cls.get();
		}

		return Window_cls;

	}
}

std::unique_ptr<WindowPlatform_cls> WindowPlatform_cls::CreateNewWindow(WindowHandle _Handle)
{
	return std::make_unique<WindowType>(_Handle);
}

void WindowPlatform_cls::SetJoystickThreshold(float _Threshold)
{
	m_JoystickThreshold = _Threshold;
}

std::optional<Event_cls> WindowPlatform_cls::WaitEvent(Time _TimeOut)
{
	const auto timeOutDuration = [_TimeOut, startTime = std::chrono::steady_clock::now()]
	{
		const bool infiniteTimeOut = _TimeOut == Time::m_ZeroTime;
		return ((!infiniteTimeOut) && ((std::chrono::steady_clock::now() - startTime) >= _TimeOut.GetDuration()));
	};

	if (m_Events.empty())
	{
		PopulateEventQueue();
	}

	while((m_Events.empty()) && (!timeOutDuration()))
	{
		Sleep(Milliseconds(10));
		PopulateEventQueue();
	}

	return PopEvent();
}

std::optional<Event_cls> WindowPlatform_cls::PollEvent()
{
	if(m_Events.empty())
	{
		PopulateEventQueue();
	}
	return PopEvent();
}

void WindowPlatform_cls::PushEvent(const Event_cls& _Event)
{
	m_Events.push(_Event);
}

std::optional<Event_cls> WindowPlatform_cls::PopEvent()
{
	std::optional<Event_cls> event;

	if(!m_Events.empty())
	{
		event.emplace(m_Events.front());
		m_Events.pop();
	}

	return event;
}

void WindowPlatform_cls::ProcessJoystickEvents()
{
	JoystickManager_cls::GetInstance().Update();

	for (unsigned int joystickIndex = 0; joystickIndex < JoystickCount; joystickIndex++)
	{
		const JoystickState previousState = m_JoystickStates->States[joystickIndex];
		m_JoystickStates->States[joystickIndex] = JoystickManager_cls::GetInstance().GetState(joystickIndex);

		const bool connected = m_JoystickStates->States[joystickIndex].Connected;
		
		if (previousState.Connected != connected)
		{
			if (connected)
			{
				PushEvent(Event_cls::JoystickConnected_str{ joystickIndex });
				m_PreviousAxes[joystickIndex].fill(0.f);
			}
			else
			{
				PushEvent(Event_cls::JoystickDisconnected_str{ joystickIndex });
			}
		}

		if (connected)
		{
			const JoystickData data = JoystickManager_cls::GetInstance().GetJoystickData(joystickIndex);

			for (unsigned int axisIndex = 0; axisIndex < JoystickAxisCount; axisIndex++)
			{
				const auto axis = static_cast<JoystickAxis_e>(axisIndex);

				if (data.Axes[axis])
				{
					const float prevPos = m_PreviousAxes[joystickIndex][axis];
					const float currPos = m_JoystickStates->States[joystickIndex].Axes[axis];

					if (std::abs(currPos - prevPos) >= m_JoystickThreshold)
					{
						PushEvent(Event_cls::JoystickMoved_str{ joystickIndex, axis, currPos });
						m_PreviousAxes[joystickIndex][axis] = currPos;
					}
				}
			}

			for (unsigned int buttonIndex = 0; buttonIndex = data.ButtonCount; buttonIndex++)
			{
				const bool prevPressed = previousState.Buttons[buttonIndex];
				const bool currPressed = m_JoystickStates->States[joystickIndex].Buttons[buttonIndex];

				if (prevPressed != currPressed)
				{
					if (currPressed)
					{
						PushEvent(Event_cls::JoystickButtonPressed_str{ joystickIndex, buttonIndex });
					}
					else
					{
						PushEvent(Event_cls::JoystickButtonReleased_str{ joystickIndex, buttonIndex });
					}
				}
			}
			
		}
	}
}

void WindowPlatform_cls::ProcessSensorEvents()
{
	SensorManager_cls::GetInstance().Updatde();

	for (unsigned int sensorIndex = 0; sensorIndex < SensorTypeCount; sensorIndex++)
	{
		const auto sensorType = static_cast<SensorType_e>(sensorIndex);

		if (SensorManager_cls::GetInstance().IsEnabled(sensorType))
		{
			const Vector3f previousValue = m_SensorValue[sensorType];
			m_SensorValue[sensorType] = SensorManager_cls::GetInstance().GetValue(sensorType);

			if (m_SensorValue[sensorType] != previousValue)
			{
				PushEvent(Event_cls::SensorChanged_str{ sensorType, m_SensorValue[sensorType] });
			}
		}
	}
}

void WindowPlatform_cls::PopulateEventQueue()
{
	ProcessJoystickEvents();
	ProcessSensorEvents();
	ProcessEvents();
}

bool WindowPlatform_cls::CreateVulkanSurface(const VkInstance &_Instance, VkSurfaceKHR _Surface, const VkAllocationCallbacks *_Allocator) const
{
#if defined(VULKAN_IMPLEMENTATION_NOT_AVAILABLE)
    return false;
#else
	return CreateVulkanPlatformSurface(_Instance, GetNativeHandle(), _Surface, _Allocator);
#endif
}
