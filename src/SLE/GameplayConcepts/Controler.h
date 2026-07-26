#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

enum class InputState
{
	Released = GLFW_RELEASE,
	Pressed = GLFW_PRESS,
	Repteated = GLFW_REPEAT
};

class Controler
{
public:
	Controler();
	~Controler();

protected:

};