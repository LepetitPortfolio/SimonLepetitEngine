#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../Common/Vertex.h"

#include "VulkanStructs.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>

const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

#if defined(NDEBUG) 
const bool EnableValidationLayers = true;
#elif defined(_DEBUG) 
const bool EnableValidationLayers = true;
#else
const bool EnableValidationLayers = false;
#endif // !NDEBUG

class VulkanPlatform
{
public :

	~VulkanPlatform();

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
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;

	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

	VkCommandPool m_CommandPool;

	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;

	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMemory;
	std::vector<void*> m_UniformBufferMapped;

	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	std::vector<VkCommandBuffer> m_CommandBuffers;

	std::vector <VkSemaphore> m_ImageAvailableSemaphores;
	std::vector <VkSemaphore> m_RenderFinishedSemaphores;
	std::vector <VkFence> m_InFlightFences;

	size_t m_CurrentFrame = 0;

	bool m_FrameBufferResized = false;

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

	void CreateDescriptorSetLayout();

	void CreateGraphicsPipeline();

	VkShaderModule CreateShaderModule(const std::vector<char>& _ShaderCode);

	void CreateFramebuffers();

	void CreateCommandPool();

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateUniformBuffers();

	void CreateBuffer(VkDeviceSize _DeviceSize, VkBufferUsageFlags _UsageFlags, VkMemoryPropertyFlags _MemoryPropertyFlags, VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory);

	void CopyBuffer(VkBuffer _SrcBuffer, VkBuffer _DstBuffer, VkDeviceSize _Size);

	uint32_t FindMemoryType(uint32_t _TypeFilter, VkMemoryPropertyFlags _MemoryProperty);

	void CreateDescriptorPool();

	void CreateDescriptorSets();

	void CreateCommandBuffers();

	void CreateSyncObjects();

	void RecordCommandBuffer(VkCommandBuffer _CommandBuffer, uint32_t _ImageIndex);

	void MainLoop();

	void DrawFrame();

	void UpdateUniformBuffer(uint32_t _CurrentImage);

	void CleanupSwapChain();

	void RecreateSwapChain();

	void Cleanup();

	bool CheckValidationLayerSupport();

	std::vector<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT _MessageType,
														const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData);

	static void FrameBufferResizeCallback(GLFWwindow* _Window, int _Width, int _Heigth);

};