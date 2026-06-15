#include "WindowPlatform.h"
#include "VulkanPlatform.h"

WindowPlatform::~WindowPlatform()
{
	Cleanup();
}

void WindowPlatform::SetVulkanPlatform(VulkanPlatform* _VulkanPlatform)
{
	if((_VulkanPlatform != nullptr) && (_VulkanPlatform != m_VulkanPlatform))
	{
		m_VulkanPlatform = _VulkanPlatform;
	}
}

void WindowPlatform::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, FrameBufferResizeCallback);

}

void WindowPlatform::Cleanup()
{
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}


void WindowPlatform::FrameBufferResizeCallback(GLFWwindow* _Window, int _Width, int _Heigth)
{
	auto app = reinterpret_cast<WindowPlatform*>(glfwGetWindowUserPointer(_Window));

	if (app->m_VulkanPlatform != nullptr)
	{
		app->m_VulkanPlatform->SetFrameBufferResized(true);
	}
}