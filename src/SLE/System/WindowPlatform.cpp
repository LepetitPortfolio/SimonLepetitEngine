#include "WindowPlatform.h"

#include <stdexcept>


WindowPlatform::~WindowPlatform()
{
	Cleanup();
}

void WindowPlatform::InitWindow(int _Width, int _Height, std::string _Name)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_Window = glfwCreateWindow(_Width, _Height, _Name.c_str(), nullptr, nullptr);
	m_Width = _Width;
	m_Height = _Height;

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, FrameBufferResizeCallback);
}

void WindowPlatform::Cleanup()
{
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}

void WindowPlatform::CreateWindowSurface(VkInstance _Instance, VkSurfaceKHR* _Surface)
{
	if (glfwCreateWindowSurface(_Instance, m_Window, nullptr, _Surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface");
	}
}


void WindowPlatform::FrameBufferResizeCallback(GLFWwindow* _Window, int _Width, int _Height)
{
	auto app = reinterpret_cast<WindowPlatform*>(glfwGetWindowUserPointer(_Window));

	if (app != nullptr)
	{
		app->m_FramebufferResized = true;
		app->m_Width = _Width;
		app->m_Height = _Height;
	}
}