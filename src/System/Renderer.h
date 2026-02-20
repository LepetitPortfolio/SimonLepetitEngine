#pragma once
#include "Platform.h"

#include <vector>
#include <string>

class Window;

class Renderer
{
public :
	Renderer();
	~Renderer();

	Window* OpenWindow(uint32_t _XSize, uint32_t _YSize, std::string _Name);

	bool Run();

private:

	Window* m_Window = nullptr;
};
