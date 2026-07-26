#pragma once
#include <GLFW/glfw3.h>

#define GLFORCE_RADIANS
#include <glm/glm.hpp>

#include "InputCommon.h"

enum class MouseButton : InputBase
{
	Unknown = -1,
	Left = GLFW_MOUSE_BUTTON_LEFT,
	Right = GLFW_MOUSE_BUTTON_RIGHT,
	Middle = GLFW_MOUSE_BUTTON_MIDDLE,
	Extra1 = GLFW_MOUSE_BUTTON_4,
	Extra2 = GLFW_MOUSE_BUTTON_5
};

enum class MouseWheel : InputBase
{
	Unknown = -1,
	Vertical = 0,
	Horizontal
};

class Mouse
{
public:

	//static glm::vec2 GetMousePositionOnScreen();
	static glm::vec2 GetMousePositionOnWindow();

	static bool GetMouseButtonDown(MouseButton _MouseButton);
	static bool GetMouseButtonUp(MouseButton _MouseButton);
	static InputStatus GetMouseButtonStatu(MouseButton _MouseButton);
};