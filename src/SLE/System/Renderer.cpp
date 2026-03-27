/*#include "PlatformConfig.h"

#include "Renderer.h"
#include "Window_cls/Window_cls.h"

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

Window_cls* Renderer::OpenWindow(uint32_t _XSize, uint32_t _YSize, std::string _Name)
{
	m_Window = new Window_cls(_XSize, _YSize, _Name);
	return m_Window;
}

bool Renderer::Run()
{
	if (m_Window)
	{
		return m_Window->Update();
	}
	return true;
}*/