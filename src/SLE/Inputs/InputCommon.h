#pragma once
#include <GLFW/glfw3.h>

using InputBase = int;

enum class ControllerType
{
	Unknown = -1,
	Keyboard = 0,
	Mouse = 1,
	Gamepad = 2
};

enum class InputStatus
{
	UnknownInput = GLFW_KEY_UNKNOWN,
	Released = GLFW_RELEASE,
	Pressed = GLFW_PRESS,
	Repeated = GLFW_REPEAT
};

enum class InputActionType
{
	UnknownInput = -1,
	Button = 0,
	Axis = 1,
	//Trigger = 2
};
