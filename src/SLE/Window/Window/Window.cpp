#pragma once
#include "Window.h"

#include "../Utils/Error.h"
#include "../Inputs/JoystickManager.h"
#include "../Inputs/SensorManager.h"

struct WindowJoystickStates
{
	std::array<JoystickState, JoystickCount> States{};
};

Window* Window::m_WindowInstance = nullptr;

Window::Window() : m_JoystickStates(std::make_unique<WindowJoystickStates>())
{
	JoystickManager::GetInstance().Update();
	
	for (unsigned int joystickIndex = 0; joystickIndex < JoystickCount; joystickIndex++)
	{
		m_JoystickStates->States[joystickIndex] = JoystickManager::GetInstance().GetState(joystickIndex);
		m_PreviousAxes[joystickIndex].fill(0.f);
	}

	for (Vector3f& sensorValue : m_SensorValue)
	{
		sensorValue = Vector3f();
	}
}

Window::~Window() 
{
	if (m_WindowInstance == this)
	{
		m_WindowInstance = nullptr;
	}
}

std::unique_ptr<Window> Window::CreateNewWindow(WindowConfig _WindowConfig, const std::string& _Title, std::uint32_t _Style, WindowState _State, const WindowSettings& _WindowSettings)
{
	if (_State == WindowState::Fullscreen)
	{
		if (m_WindowInstance != nullptr)
		{
			Err() << "A window already exists, only one window can be created at a time." << std::endl;
			_State = WindowState::Windowed;
		}
		else if (!_WindowConfig.IsValid())
		{
			Err() << "The requested video mode is not valid, window creation failed." << std::endl;
			assert(!WindowConfig::GetFullscreenModes().empty() && "No window config available");
			_WindowConfig = WindowConfig::GetFullscreenModes()[0];

			Err() << "WindowConfig { Size : { " << _WindowConfig.m_Size.X << ", " << _WindowConfig.m_Size.Y << " }, bitsPerPixel: " << _WindowConfig.m_BitsPerPixel << " }" << std::endl;
		}

		if ((_Style & static_cast<std::uint32_t>(WindowStyle::Close)) || (_Style & static_cast<std::uint32_t>(WindowStyle::Resize)))
		{
			_Style |= static_cast<std::uint32_t>(WindowStyle::Titlebar);
		}

		auto window = std::make_unique<WindowType>(_WindowConfig, _Title, _Style, _State, _WindowSettings);

		if(_State == WindowState::Fullscreen)
		{
			m_WindowInstance = window.get();
		}

		return window;

	}
}

std::unique_ptr<Window> Window::CreateNewWindow(WindowHandle _Handle)
{
	return std::make_unique<WindowType>(_Handle);
}

void Window::SetJoystickThreshold(float _Threshold)
{
	m_JoystickThreshold = _Threshold;
}

std::optional<Event> Window::WaitEvent(Time _TimeOut)
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

std::optional<Event> Window::PollEvent()
{
	if(m_Events.empty())
	{
		PopulateEventQueue();
	}
	return PopEvent();
}

void Window::PushEvent(const Event& _Event)
{
	m_Events.push(_Event);
}

std::optional<Event> Window::PopEvent()
{
	std::optional<Event> event;

	if(!m_Events.empty())
	{
		event.emplace(m_Events.front());
		m_Events.pop();
	}

	return event;
}

void Window::ProcessJoystickEvents()
{
	JoystickManager::GetInstance().Update();

	for (unsigned int joystickIndex = 0; joystickIndex < JoystickCount; joystickIndex++)
	{
		const JoystickState previousState = m_JoystickStates->States[joystickIndex];
		m_JoystickStates->States[joystickIndex] = JoystickManager::GetInstance().GetState(joystickIndex);

		const bool connected = m_JoystickStates->States[joystickIndex].Connected;
		
		if (previousState.Connected != connected)
		{
			if (connected)
			{
				PushEvent(Event::JoystickConnected{ joystickIndex });
				m_PreviousAxes[joystickIndex].fill(0.f);
			}
			else
			{
				PushEvent(Event::JoystickDisconnected{ joystickIndex });
			}
		}

		if (connected)
		{
			const JoystickData data = JoystickManager::GetInstance().GetJoystickData(joystickIndex);

			for (unsigned int axisIndex = 0; axisIndex < JoystickAxisCount; axisIndex++)
			{
				const auto axis = static_cast<JoystickAxis>(axisIndex);

				if (data.Axes[axis])
				{
					const float prevPos = m_PreviousAxes[joystickIndex][axis];
					const float currPos = m_JoystickStates->States[joystickIndex].Axes[axis];

					if (std::abs(currPos - prevPos) >= m_JoystickThreshold)
					{
						PushEvent(Event::JoystickMoved{ joystickIndex, axis, currPos });
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
						PushEvent(Event::JoystickButtonPressed{ joystickIndex, buttonIndex });
					}
					else
					{
						PushEvent(Event::JoystickButtonReleased{ joystickIndex, buttonIndex });
					}
				}
			}
			
		}
	}
}

void Window::ProcessSensorEvents()
{
	SensorManager::GetInstance().Updatde();

	for (unsigned int sensorIndex = 0; sensorIndex < SensorTypeCount; sensorIndex++)
	{
		const auto sensorType = static_cast<SensorType>(sensorIndex);

		if (SensorManager::GetInstance().IsEnabled(sensorType))
		{
			const Vector3f previousValue = m_SensorValue[sensorType];
			m_SensorValue[sensorType] = SensorManager::GetInstance().GetValue(sensorType);

			if (m_SensorValue[sensorType] != previousValue)
			{
				PushEvent(Event::SensorChanged{ sensorType, m_SensorValue[sensorType] });
			}
		}
	}
}

void Window::PopulateEventQueue()
{
	ProcessJoystickEvents();
	ProcessSensorEvents();
	ProcessEvents();
}