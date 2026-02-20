#include "Platform.h"

#include "Renderer.h"
#include "Window/Window.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	if (m_Window)
	{
		delete m_Window;
		m_Window = nullptr;
	}
}

Window* Renderer::OpenWindow(uint32_t _XSize, uint32_t _YSize, std::string _Name)
{
	m_Window = new Window(_XSize, _YSize, _Name);
	return m_Window;
}

bool Renderer::Run()
{
	if (m_Window)
	{
		return m_Window->Update();
	}
	return true;
}