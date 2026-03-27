#include "JoystickManager_cls.h"

#include <cassert>

JoystickManager_cls& JoystickManager_cls::GetInstance()
{
	static JoystickManager_cls instance;
	return instance;
}

const JoystickData& JoystickManager_cls::GetJoystickData(unsigned int _JoystickIndex) const
{
	assert(_JoystickIndex < JoystickCount && "Joystick index must be less than JoystickCount");
	return m_Joysticks[_JoystickIndex].Data;
}

const JoystickState& JoystickManager_cls::GetState(unsigned int _JoystickIndex) const
{
	assert(_JoystickIndex < JoystickCount && "Joystick index must be less than JoystickCount");
	return m_Joysticks[_JoystickIndex].State;
}

const JoystickIdentification_e& JoystickManager_cls::GetIdentification(unsigned int _JoystickIndex) const
{
	assert(_JoystickIndex < JoystickCount && "Joystick index must be less than JoystickCount");
	return m_Joysticks[_JoystickIndex].Identification;
}

void JoystickManager_cls::Update()
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
				item.Identification = JoystickIdentification_e();
			}
		}
		else
		{
			if (JoystickPlatform::IsConnected(itemIndex))
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

JoystickManager_cls::JoystickManager_cls()
{
	JoystickPlatform::Initialize();
}

JoystickManager_cls::~JoystickManager_cls()
{
	for (Item& item : m_Joysticks)
	{
		if (item.State.Connected)
		{
			item.Joystick.Close();
		}
	}

	JoystickPlatform::Cleanup();
}

