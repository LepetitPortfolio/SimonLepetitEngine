#pragma once
#include <GLFW/glfw3.h>

#include "InputCommon.h"

enum class GamepadButton : InputBase
{
	Unknown = -1,
	A = GLFW_GAMEPAD_BUTTON_A,
	B = GLFW_GAMEPAD_BUTTON_B,
	Y = GLFW_GAMEPAD_BUTTON_Y,
	X = GLFW_GAMEPAD_BUTTON_X,
	U,
	V,
	PovX,
	PovY
};

enum class GamepadAxis : InputBase
{
	Unknown = -1,
	Right_Axis_X = GLFW_GAMEPAD_AXIS_RIGHT_X,
	Right_Axis_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,
	Left_Axis_X = GLFW_GAMEPAD_AXIS_LEFT_X,
	Left_Axis_Y = GLFW_GAMEPAD_AXIS_LEFT_Y,
	Rigth_Trigger_Axis = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
	Left_Trigger_Axis = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
	
};
