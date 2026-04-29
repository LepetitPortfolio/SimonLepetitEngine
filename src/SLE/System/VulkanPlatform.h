#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanStructs.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>


class VulkanPlatform
{
public :
	void Run();
	
private:

	GLFWwindow* m_Window;

	VkInstance m_Instance;

	VkDebugUtilsMessengerEXT m_DebugMessenger;

	VkSurfaceKHR m_Surface;

	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_Device;

	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;

	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;

	std::vector<VkImageView> m_SwapChainImageViews;

	VkRenderPass m_RenderPass;

	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;

	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

	VkCommandPool m_CommandPool;

	VkCommandBuffer m_CommandBuffer;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	VkSemaphore m_ImageAvailableSemaphore;
	VkSemaphore m_RenderFinishedSemaphore;

	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation"	};

	uint32_t m_Width = 800;
	uint32_t m_Height = 600;

#ifndef NDEBUG
	const bool m_EnableValidationLayers = false;
#else
	const bool m_EnableValidationLayers = true;
#endif // !NDEBUG


	void InitWindow();

	void InitVulkan();

	void CreateInstance();

	void SetupDebugMessenger();

	void PopulateDebugMessagerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _MessengerCreateInfo);

	void CreateSurface();

	void PickPhysicalDevice();

	bool IsDeviceSuitable(VkPhysicalDevice _Device);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice _Device);

	int RateDeviceSuitability(VkPhysicalDevice _Device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice _Device);

	void CreateLogicalDevice();

	void CreateSwapChain();

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _Device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _AvailableFormats);

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _AvailablePresentModes);

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _Capabilities);

	void CreateImageViews();

	void CreateRenderPass();

	void CreateGraphicsPipeline();

	VkShaderModule CreateShaderModule(const std::vector<char>& _ShaderCode);

	void CreateFramebuffers();

	void CreateCommandPool();

	void CreateCommandBuffers();

	void CreateSemaphores();

	void RecordCommandBuffer(VkCommandBuffer _CommandBuffer, uint32_t _ImageIndex);

	void MainLoop();

	void DrawFrame();

	void Cleanup();

	bool CheckValidationLayerSupport();

	std::vector<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT _MessageType,
														const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData);


};