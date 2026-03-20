#include "Joystick.h"
#include "Joystick.h"
#include "Joystick.h"
#include "Joystick.h"
#include "Joystick.h"
#include "JoystickManager.h"

#include <cassert>

bool Joystick::IsConnected(unsigned int _JoystickIndex)
{
	return JoystickManager::GetInstance().GetState(_JoystickIndex).Connected;
}

unsigned int Joystick::GetButtonCount(unsigned int _JoystickIndex) const
{
	return JoystickManager::GetInstance().GetJoystickData(_JoystickIndex).ButtonCount;
}

bool Joystick::HasAxis(unsigned int _JoystickIndex, JoystickAxis _Axis) const
{
	return JoystickManager::GetInstance().GetJoystickData().Axes[_Axis];
}

bool Joystick::IsButtonPressed(unsigned int _JoystickIndex, unsigned int _Button) const
{
	assert(_Button < JoystickButtonCount && "Button must be less than JoystickButtonCount");
	return JoystickManager::GetInstance().GetState(_JoystickIndex).Buttons[_Button];
}

float Joystick::GetAxisPosition(unsigned int _JoystickIndex, JoystickAxis _Axis) const
{
	return JoystickManager::GetInstance().GetState(_JoystickIndex).Axes[_Axis];
}

void Joystick::Update()
{
	JoystickManager::GetInstance().Update();
}


