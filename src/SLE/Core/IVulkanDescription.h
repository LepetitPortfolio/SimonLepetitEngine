#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include <vector>

struct IVulkanDescription
{
public :
	static std::vector<VkVertexInputBindingDescription> GetBindingDescription(){ return std::vector<VkVertexInputBindingDescription>(0); }

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(){ return std::vector<VkVertexInputAttributeDescription>(0); };	
};