#include "JoystickManager.h"

#include <cassert>

JoystickManager& JoystickManager::GetInstance()
{
	static JoystickManager instance;
	return instance;
}

const JoystickData& JoystickManager::GetJoystickData(unsigned int _JoystickIndex) const
{
	assert(_JoystickIndex < JoystickCount && "Joystick index must be less than JoystickCount");
	return m_Joysticks[_JoystickIndex].Data;
}

const JoystickState& JoystickManager::GetState(unsigned int _JoystickIndex) const
{
	assert(_JoystickIndex < JoystickCount && "Joystick index must be less than JoystickCount");
	return m_Joysticks[_JoystickIndex].State;
}

const JoystickIdentification& JoystickManager::GetIdentification(unsigned int _JoystickIndex) const
{
	assert(_JoystickIndex < JoystickCount && "Joystick index must be less than JoystickCount");
	return m_Joysticks[_JoystickIndex].Identification;
}

void JoystickManager::Update()
{
	for (unsigned int itemIndex = 0; itemIndex < JoystickCount; itemIndex++)
	{
		Item& item = m_Joysticks[itemIndex];

		if (item.State.Connected)
		{
			item.State = item.Joystick.Update();

			if (!item.State.Connected)
			{
				item.Joystick.Close();
				item.Data = JoystickData();
				item.State = JoystickState();
				item.Identification = JoystickIdentification();
			}
		}
		else
		{
			if (Joystick::IsConnected(itemIndex))
			{
				if (item.Joystick.Open(itemIndex))
				{
					item.Data = item.Joystick.GetJoystickData();
					item.State = item.Joystick.Update();
					item.Identification = item.Joystick.GetIdentification();
				}
			}
		}
	}
}

JoystickManager::JoystickManager()
{
	Joystick::Initialize();
}

JoystickManager::~JoystickManager()
{
	for (Item& item : m_Joysticks)
	{
		if (item.State.Connected)
		{
			item.Joystick.Close();
		}
	}

	Joystick::Cleanup();
}

