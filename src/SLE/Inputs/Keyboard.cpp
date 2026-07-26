#include "Keyboard.h"
#include "../Core/GlobalFunctionLibrary.h"

bool Keyboard::GetKeyInputDown(KeyboardInput _KeyboardInput)
{
	if (glfwGetKey(GlobalFunctionLibrary::GetWindow(), (int)_KeyboardInput) == (int)InputStatus::Pressed)
	{
		return true;
	}
	return false;
}

bool Keyboard::GetKeyInputUp(KeyboardInput _KeyboardInput)
{
	if (glfwGetKey(GlobalFunctionLibrary::GetWindow(), (int)_KeyboardInput) == (int)InputStatus::Released)
	{
		return true;
	}
	return false;
}

InputStatus Keyboard::GetKeyInputStatut(KeyboardInput _KeyboardInput)
{
	return (InputStatus)glfwGetKey(GlobalFunctionLibrary::GetWindow(), (int)_KeyboardInput);
}
