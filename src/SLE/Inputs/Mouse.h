#pragma once
#include <GLFW/glfw3.h>

enum class MouseButton
{
	Unknown = -1,
	Left = GLFW_MOUSE_BUTTON_LEFT,
	Right = GLFW_MOUSE_BUTTON_RIGHT,
	Middle = GLFW_MOUSE_BUTTON_MIDDLE,
	Extra1 = GLFW_MOUSE_BUTTON_4,
	Extra2 = GLFW_MOUSE_BUTTON_5
};

enum class MouseWheel
{
	Unknown = -1,
	Vertical = 0,
	Horizontal
};
