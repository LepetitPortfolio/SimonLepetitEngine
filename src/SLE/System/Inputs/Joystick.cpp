#include "Joystick.h"
#include "JoystickManager_cls.h"

#include <cassert>

bool Joystick::IsConnected(unsigned int _JoystickIndex)
{
	return JoystickManager_cls::GetInstance().GetState(_JoystickIndex).Connected;
}

unsigned int Joystick::GetButtonCount(unsigned int _JoystickIndex) const
{
	return JoystickManager_cls::GetInstance().GetJoystickData(_JoystickIndex).ButtonCount;
}

bool Joystick::HasAxis(unsigned int _JoystickIndex, JoystickAxis_e _Axis) const
{
	return JoystickManager_cls::GetInstance().GetJoystickData(_JoystickIndex).Axes[_Axis];
}

bool Joystick::IsButtonPressed(unsigned int _JoystickIndex, unsigned int _Button) const
{
	assert(_Button < JoystickButtonCount && "Button must be less than JoystickButtonCount");
	return JoystickManager_cls::GetInstance().GetState(_JoystickIndex).Buttons[_Button];
}

float Joystick::GetAxisPosition(unsigned int _JoystickIndex, JoystickAxis_e _Axis) const
{
	return JoystickManager_cls::GetInstance().GetState(_JoystickIndex).Axes[_Axis];
}

void Joystick::Update()
{
	JoystickManager_cls::GetInstance().Update();
}


