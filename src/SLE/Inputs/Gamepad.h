#pragma once
#include <GLFW/glfw3.h>

enum class Gamepad
{
	Unknown = -1,
	X = GLFW_GAMEPAD_BUTTON_A,
	Y,
	Z,
	R,
	U,
	V,
	PovX,
	PovY
};
