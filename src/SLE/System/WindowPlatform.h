#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanPlatform;

class WindowPlatform
{
public:
	~WindowPlatform();

	GLFWwindow* GetWindow() { return m_Window; }

	void SetVulkanPlatform(VulkanPlatform* _VulkanPlatform);
	void InitWindow();
	void Cleanup();

private:
	GLFWwindow* m_Window;

	VulkanPlatform* m_VulkanPlatform = nullptr;

	static void FrameBufferResizeCallback(GLFWwindow* _Window, int _Width, int _Heigth);
};