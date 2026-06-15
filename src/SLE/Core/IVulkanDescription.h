#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include <vector>

struct IVulkanDescription
{
public :
	static VkVertexInputBindingDescription GetBindingDescription(){ return VkVertexInputBindingDescription{}; }

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(){ return std::vector<VkVertexInputAttributeDescription>(); };	
};