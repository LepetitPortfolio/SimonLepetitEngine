#include "VulkanPlatform.h"
#include "WindowPlatform.h"

#include "../Common/Error.h"

#include <set>
#include <unordered_set>


VkResult CreateDebugUntilsMessageEXT(VkInstance _Instance, const VkDebugUtilsMessengerCreateInfoEXT* _pCreateInfo, const VkAllocationCallbacks* _pAllocator, VkDebugUtilsMessengerEXT* _pCallback)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		return func(_Instance, _pCreateInfo, _pAllocator, _pCallback);
	}

	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance _Instance, const VkAllocationCallbacks* _pAllocator, VkDebugUtilsMessengerEXT _Callback)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(_Instance, _Callback, _pAllocator);
	}
}


VulkanPlatform::VulkanPlatform(WindowPlatform& _WindowPlatform) : m_WindowPlatform(_WindowPlatform)
{
	m_VulkanData = std::make_unique<VulkanData>();

	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateCommandPool();
}

VulkanPlatform::~VulkanPlatform()
{
	vkDestroyCommandPool(m_VulkanData->Device, m_VulkanData->CommandPool, nullptr);

	vkDestroyDevice(m_VulkanData->Device, nullptr);

	if (m_EnableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_VulkanData->Instance, nullptr, m_VulkanData->DebugMessenger);
	}

	vkDestroySurfaceKHR(m_VulkanData->Instance, m_VulkanData->Surface, nullptr);
	vkDestroyInstance(m_VulkanData->Instance, nullptr);
}

uint32_t VulkanPlatform::FindMemoryType(uint32_t _TypeFilter, VkMemoryPropertyFlags _MemoryProperty)
{
	VkPhysicalDeviceMemoryProperties memoryProperties{};
	vkGetPhysicalDeviceMemoryProperties(m_VulkanData->PhysicalDevice, &memoryProperties);

	uint32_t memoryTypeIndex = 0;
	uint32_t outMemoryType = 0;
	bool checkValid = false;
	while ((!checkValid) && (memoryTypeIndex < memoryProperties.memoryTypeCount))
	{
		if ((_TypeFilter & (1 << memoryTypeIndex)) && ((memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & _MemoryProperty) == _MemoryProperty))
		{
			outMemoryType = memoryTypeIndex;
			checkValid = true;
		}

		memoryTypeIndex++;
	}

	if (!checkValid)
	{
		Err() << "failed to find suitable memory type!" << std::endl;
	}

	return outMemoryType;
}

VkFormat VulkanPlatform::FindSupportedFormat(const std::vector<VkFormat>& _Candidates, VkImageTiling _Tiling, VkFormatFeatureFlags _Features)
{
	VkFormat outFormat = VK_FORMAT_UNDEFINED;
	int formatIndex = 0;
	while ((outFormat == VK_FORMAT_UNDEFINED) && (formatIndex < _Candidates.size()))
	{
		VkFormat candidate = _Candidates[formatIndex];
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_VulkanData->PhysicalDevice, candidate, &props);

		if ((_Tiling == VK_IMAGE_TILING_LINEAR) && ((props.linearTilingFeatures & _Features) == _Features))
		{
			outFormat = candidate;
		}
		else if ((_Tiling == VK_IMAGE_TILING_OPTIMAL) && ((props.optimalTilingFeatures & _Features) == _Features))
		{
			outFormat = candidate;
		}
		else
		{
			formatIndex++;
		}
	}

	if (outFormat == VK_FORMAT_UNDEFINED)
	{
		Err() << "failed to find supported format!" << std::endl;
	}

	return outFormat;
}


void VulkanPlatform::CreateBuffer(VkDeviceSize _DeviceSize, VkBufferUsageFlags _UsageFlags, VkMemoryPropertyFlags _MemoryPropertyFlags, VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = _DeviceSize;
	bufferCreateInfo.usage = _UsageFlags;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_VulkanData->Device, &bufferCreateInfo, nullptr, &_Buffer) != VK_SUCCESS)
	{
		Err() << "failed to create buffer!" << std::endl;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(m_VulkanData->Device, _Buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, _MemoryPropertyFlags);

	if (vkAllocateMemory(m_VulkanData->Device, &memoryAllocateInfo, nullptr, &_BufferMemory))
	{
		Err() << "ffailed to allocate vertex buffer memory!" << std::endl;
	}

	vkBindBufferMemory(m_VulkanData->Device, _Buffer, _BufferMemory, 0);
}

void VulkanPlatform::CopyBuffer(VkBuffer _SrcBuffer, VkBuffer _DstBuffer, VkDeviceSize _Size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = _Size;

	vkCmdCopyBuffer(commandBuffer, _SrcBuffer, _DstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VulkanPlatform::BeginSingleTimeCommands()
{

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandPool = m_VulkanData->CommandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_VulkanData->Device, &commandBufferAllocateInfo, &commandBuffer);
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	return commandBuffer;
}

void VulkanPlatform::EndSingleTimeCommands(VkCommandBuffer _CommandBuffer)
{
	vkEndCommandBuffer(_CommandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_CommandBuffer;

	vkQueueSubmit(m_VulkanData->GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_VulkanData->GraphicsQueue);

	vkFreeCommandBuffers(m_VulkanData->Device, m_VulkanData->CommandPool, 1, &_CommandBuffer);
}

void VulkanPlatform::CopyBufferToImage(VkBuffer _Buffer, VkImage _Image, uint32_t _Width, uint32_t _Height, uint32_t _LayerCount)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = _LayerCount;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { _Width, _Height, 1 };

	vkCmdCopyBufferToImage(	commandBuffer, _Buffer,	_Image,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	EndSingleTimeCommands(commandBuffer);
}

void VulkanPlatform::CreateImageWithInfo(const VkImageCreateInfo& _ImageInfo, VkMemoryPropertyFlags _Properties, VkImage& _Image, VkDeviceMemory& _ImageMemory)
{
	if (vkCreateImage(m_VulkanData->Device, &_ImageInfo, nullptr, &_Image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_VulkanData->Device, _Image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _Properties);

	if (vkAllocateMemory(m_VulkanData->Device, &allocInfo, nullptr, &_ImageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	if (vkBindImageMemory(m_VulkanData->Device, _Image, _ImageMemory, 0) != VK_SUCCESS) {
		throw std::runtime_error("failed to bind image memory!");
	}
}


void VulkanPlatform::CreateInstance()
{
	if ((m_EnableValidationLayers) && (!CheckValidationLayerSupport()))
	{
		Err() << "validation layers requested, but not available!" << std::endl;
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	auto extensions = GetRequiredExtensions();

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};

	if (m_EnableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		PopulateDebugMessagerCreateInfo(debugMessengerCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_VulkanData->Instance) != VK_SUCCESS)
	{
		Err() << "failed to create Vulkan instance!" << std::endl;
	}
}

bool VulkanPlatform::CheckValidationLayerSupport()
{
	uint32_t layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	bool bBreak = false;
	bool layerFound = false;

	int validationLayerIndex = 0;

	while ((!bBreak) && (validationLayerIndex < m_ValidationLayers.size()))
	{
		int availableLayerIndex = 0;
		layerFound = false;

		while ((!layerFound) && (availableLayerIndex < availableLayers.size()))
		{
			if (strcmp(m_ValidationLayers[validationLayerIndex], availableLayers[availableLayerIndex].layerName) == 0)
			{
				layerFound = true;
			}
			else
			{
				availableLayerIndex++;
			}
		}

		if (!layerFound)
		{
			bBreak = true;
		}
		else
		{
			validationLayerIndex++;
		}
	}

	return layerFound;
}

std::vector<const char*> VulkanPlatform::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;

	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_EnableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


void VulkanPlatform::SetupDebugMessenger()
{
	if (!m_EnableValidationLayers)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
	PopulateDebugMessagerCreateInfo(debugUtilsMessengerCreateInfo);

	if (CreateDebugUntilsMessageEXT(m_VulkanData->Instance, &debugUtilsMessengerCreateInfo, nullptr, &m_VulkanData->DebugMessenger) != VK_SUCCESS)
	{
		Err() << "failed to set up debug messenger!" << std::endl;
	}
}

void VulkanPlatform::PopulateDebugMessagerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _MessengerCreateInfo)
{
	_MessengerCreateInfo = {};
	_MessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	_MessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	_MessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	_MessengerCreateInfo.pfnUserCallback = DebugCallback;
	_MessengerCreateInfo.pUserData = nullptr;
}

void VulkanPlatform::CreateSurface()
{
	m_WindowPlatform.CreateWindowSurface(m_VulkanData->Instance, &m_VulkanData->Surface);
}

void VulkanPlatform::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_VulkanData->Instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		Err() << "failed to find GPUs with Vulkan support!" << std::endl;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_VulkanData->Instance, &deviceCount, devices.data());

	bool bBreak = false;
	int deviceIndex = 0;

	while ((!bBreak) && (deviceIndex < devices.size()))
	{
		VkPhysicalDevice device = devices[deviceIndex];
		if (IsDeviceSuitable(device))
		{
			m_VulkanData->PhysicalDevice = device;
			m_VulkanData->MSAASamples = GetMaxUsableSampleCount();
			bBreak = true;
		}
		else
		{
			deviceIndex++;
		}
	}

	if (m_VulkanData->PhysicalDevice == VK_NULL_HANDLE)
	{
		Err() << "failed to find a suitable GPU!" << std::endl;
	}

	/*std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto device : devices)
	{
		int score = RateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}

	if (candidates.rbegin()->first > 0)
	{
		physicalDevice = candidates.rbegin()->second;
	}
	else
	{
		Err() << "failed to find a suitable GPU!" << std::endl;
	}*/

}

bool VulkanPlatform::IsDeviceSuitable(VkPhysicalDevice _Device)
{
	QueueFamilyIndices indices = FindQueueFamilies(_Device);

	bool extensionsSupported = CheckDeviceExtensionSupport(_Device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_Device);
		swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(_Device, &supportedFeatures);

	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

SwapChainSupportDetails VulkanPlatform::QuerySwapChainSupport(VkPhysicalDevice _Device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_Device, m_VulkanData->Surface, &details.Capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_Device, m_VulkanData->Surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_Device, m_VulkanData->Surface, &formatCount, details.Formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_Device, m_VulkanData->Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_Device, m_VulkanData->Surface, &presentModeCount, details.PresentModes.data());
	}

	return details;
}

bool VulkanPlatform::CheckDeviceExtensionSupport(VkPhysicalDevice _Device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(_Device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(_Device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

int VulkanPlatform::RateDeviceSuitability(VkPhysicalDevice _Device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(_Device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(_Device, &deviceFeatures);

	bool isSuitable = (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) && (deviceFeatures.geometryShader);

	if (!isSuitable)
	{
		return 0;
	}

	int score = 0;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	if (!deviceFeatures.geometryShader)
	{
		return 0;
	}

	return score;
}

QueueFamilyIndices VulkanPlatform::FindQueueFamilies(VkPhysicalDevice _Device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_Device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_Device, &queueFamilyCount, queueFamilies.data());

	int queueFamilyIndex = 0;
	bool bBreak = false;

	while ((!bBreak) && (queueFamilyIndex < queueFamilies.size()))
	{
		if (queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.GraphicsFamily = queueFamilyIndex;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(_Device, queueFamilyIndex, m_VulkanData->Surface, &presentSupport);
		if (presentSupport)
		{
			indices.PresentFamily = queueFamilyIndex;
		}

		if (indices.IsComplete())
		{
			bBreak = true;
		}

		queueFamilyIndex++;
	}


	return indices;
}

VkSampleCountFlagBits VulkanPlatform::GetMaxUsableSampleCount()
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_VulkanData->PhysicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags sampleCountFlags = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

	if (sampleCountFlags & VK_SAMPLE_COUNT_64_BIT)
	{
		return VK_SAMPLE_COUNT_64_BIT;
	}

	if (sampleCountFlags & VK_SAMPLE_COUNT_32_BIT)
	{
		return VK_SAMPLE_COUNT_32_BIT;
	}

	if (sampleCountFlags & VK_SAMPLE_COUNT_16_BIT)
	{
		return VK_SAMPLE_COUNT_16_BIT;
	}

	if (sampleCountFlags & VK_SAMPLE_COUNT_8_BIT)
	{
		return VK_SAMPLE_COUNT_8_BIT;
	}

	if (sampleCountFlags & VK_SAMPLE_COUNT_4_BIT)
	{
		return VK_SAMPLE_COUNT_4_BIT;
	}

	if (sampleCountFlags & VK_SAMPLE_COUNT_2_BIT)
	{
		return VK_SAMPLE_COUNT_2_BIT;
	}

	return VK_SAMPLE_COUNT_1_BIT;
}

void VulkanPlatform::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_VulkanData->PhysicalDevice);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };
	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (m_EnableValidationLayers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
	{
		deviceCreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_VulkanData->PhysicalDevice, &deviceCreateInfo, nullptr, &m_VulkanData->Device) != VK_SUCCESS)
	{
		Err() << "failed to create logical device!" << std::endl;
	}

	vkGetDeviceQueue(m_VulkanData->Device, indices.GraphicsFamily.value(), 0, &m_VulkanData->GraphicsQueue);
	vkGetDeviceQueue(m_VulkanData->Device, indices.PresentFamily.value(), 0, &m_VulkanData->PresentQueue);
}

void VulkanPlatform::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_VulkanData->PhysicalDevice);

	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

	if (vkCreateCommandPool(m_VulkanData->Device, &commandPoolCreateInfo, nullptr, &m_VulkanData->CommandPool) != VK_SUCCESS)
	{
		Err() << "failed to create command pool!" << std::endl;
	}
}