#include "Mouse.h"

#include "../Core/GlobalFunctionLibrary.h"

/*glm::vec2 Mouse::GetMousePositionOnScreen()
{
	glm::vec2() outPosition;
	return glm::vec2();
}*/

glm::vec2 Mouse::GetMousePositionOnWindow()
{
	glm::highp_dvec2 outPosition = glm::highp_dvec2();

	glfwGetCursorPos(GlobalFunctionLibrary::GetWindow(), &outPosition.x, &outPosition.y);

	return outPosition;
}

bool Mouse::GetMouseButtonDown(MouseButton _MouseButton)
{
	if (glfwGetMouseButton(GlobalFunctionLibrary::GetWindow(), (int)_MouseButton) == (int)InputStatus::Pressed)
	{
		return true;
	}
	return false;
}

bool Mouse::GetMouseButtonUp(MouseButton _MouseButton)
{
	if (glfwGetMouseButton(GlobalFunctionLibrary::GetWindow(), (int)_MouseButton) == (int)InputStatus::Released)
	{
		return true;
	}
	return false;
}

InputStatus Mouse::GetMouseButtonStatu(MouseButton _MouseButton)
{
	return (InputStatus)glfwGetMouseButton(GlobalFunctionLibrary::GetWindow(), (int)_MouseButton);
}
