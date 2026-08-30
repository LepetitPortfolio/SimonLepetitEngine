#include "TextureBase.h"

#include "../../Core/GlobalFunctionLibrary.h"
#include "../../System/AssetDataManager.h"
#include "../../System/VulkanIncludes.h"
#include "../../Common/Error.h"



TextureBase::TextureBase()
{
	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

TextureBase::~TextureBase()
{
	CleanupTextureBase();
}


void TextureBase::CleanupTextureBase()
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();
	if (!vulkanDevice || !vulkanDevice->GetLogicalDevice())
	{
		return;
	}

	VkDevice device = vulkanDevice->GetLogicalDevice();

	if (m_TextureImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device, m_TextureImageView, nullptr);
		m_TextureImageView = VK_NULL_HANDLE;
	}

	if (m_TextureImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(device, m_TextureImage, nullptr);
		m_TextureImage = VK_NULL_HANDLE;
	}

	if (m_TextureMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, m_TextureMemory, nullptr);
		m_TextureMemory = VK_NULL_HANDLE;
	}
}

void TextureBase::CreateTextureImageView()
{
	VulkanSwapchain* swapchain = GlobalFunctionLibrary::GetVulkanPlatform()->GetVulkanSwapchain();

	if (!swapchain)
	{
		Err() << "VulkanSwapchain is not initialized!" << std::endl;
		return;
	}

	m_TextureImageView = CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
}

void TextureBase::GenerateMipMaps(VkImage _Image, VkFormat _ImageFormat, int32_t _TexWidth, int32_t _TexHeight, uint32_t _MipLevels)
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();
	VkFormatProperties formatProperties;
	VulkanCommandManager* commandManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetCommandManager();

	vkGetPhysicalDeviceFormatProperties(vulkanDevice->GetPhysicalDevice(), _ImageFormat, &formatProperties);


	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		Err() << "texture image format does not support linear blitting!" << std::endl;
	}

	VkCommandBuffer commandBuffer = commandManager->BeginSingleTimeCommands();

	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.image = _Image;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	imageMemoryBarrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = _TexWidth;
	int32_t mipHeight = _TexHeight;

	for (uint32_t mipLevelIndex = 1; mipLevelIndex < _MipLevels; mipLevelIndex++)
	{
		// Transitionne le niveau précédent vers TRANSFER_SRC_OPTIMAL pour permettre le blit
		imageMemoryBarrier.subresourceRange.baseMipLevel = mipLevelIndex - 1;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		// Effectue le blit du niveau précédent vers le niveau actuel
		VkImageBlit imageBlit{};
		imageBlit.srcOffsets[0] = { 0, 0, 0 };
		imageBlit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.mipLevel = mipLevelIndex - 1;
		imageBlit.srcSubresource.baseArrayLayer = 0;
		imageBlit.srcSubresource.layerCount = 1;
		imageBlit.dstOffsets[0] = { 0, 0, 0 };
		imageBlit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.mipLevel = mipLevelIndex;
		imageBlit.dstSubresource.baseArrayLayer = 0;
		imageBlit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer, _Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);

		// Transitionne le niveau précédent vers SHADER_READ_ONLY_OPTIMAL
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		// Met à jour les dimensions pour le niveau suivant
		if (mipWidth > 1)
		{
			mipWidth /= 2;
		}

		if (mipHeight > 1)
		{
			mipHeight /= 2;
		}
	}

	// Transitionne le dernier niveau vers SHADER_READ_ONLY_OPTIMAL
	imageMemoryBarrier.subresourceRange.baseMipLevel = _MipLevels - 1;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	commandManager->EndSingleTimeCommands(commandBuffer);
}

void TextureBase::TransitionImageLayout(VkImage _Image, VkFormat _Format, VkImageLayout _OldLayout, VkImageLayout _NewLayout, uint32_t _MipLevels)
{
	VulkanCommandManager* commandManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetCommandManager();
	VkCommandBuffer commandBuffer = commandManager->BeginSingleTimeCommands();

	

	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = _OldLayout;
	imageMemoryBarrier.newLayout = _NewLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = _Image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = _MipLevels;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	if (_NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (HasStencilComponent(_Format))
		{
			imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else 
	{
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;


	if (_OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (_OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (_OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else 
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	commandManager->EndSingleTimeCommands(commandBuffer);
}

bool TextureBase::HasStencilComponent(VkFormat _Format)
{
	return _Format == VK_FORMAT_D32_SFLOAT_S8_UINT || _Format == VK_FORMAT_D24_UNORM_S8_UINT;
}	


void TextureBase::CopyBufferToImage(VkBuffer _Buffer, VkImage _Image, uint32_t _Width, uint32_t _Height)
{
	VulkanCommandManager* commandManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetCommandManager();
	VkCommandBuffer commandBuffer = commandManager->BeginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { _Width, _Height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, _Buffer, _Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandManager->EndSingleTimeCommands(commandBuffer);
}


void TextureBase::CreateImage(uint32_t _Width, uint32_t _Height, uint32_t _MipLevels, VkSampleCountFlagBits _NumSamples, VkFormat _Format, VkImageTiling _Tiling, VkImageUsageFlags _Usage, VkMemoryPropertyFlags _Properties, VkImage& _Image, VkDeviceMemory& _ImageMemory)
{
	VulkanDevice* vulkanDevice =  GlobalFunctionLibrary::GetVulkanDevice();

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = _Width;
	imageInfo.extent.height = _Height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = _MipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = _Format;
	imageInfo.tiling = _Tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = _Usage;
	imageInfo.samples = _NumSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(vulkanDevice->GetLogicalDevice(), &imageInfo, nullptr, &_Image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(vulkanDevice->GetLogicalDevice(), _Image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = vulkanDevice->FindMemoryType(memRequirements.memoryTypeBits, _Properties);

	if (vkAllocateMemory(vulkanDevice->GetLogicalDevice(), &allocInfo, nullptr, &_ImageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(vulkanDevice->GetLogicalDevice(), _Image, _ImageMemory, 0);
}

VkImageView TextureBase::CreateImageView(VkImage _Image, VkFormat _Format, VkImageAspectFlags _AspectFlags, uint32_t _MipLevels)
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	VkImageView imageView;
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = _Image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = _Format;
	imageViewCreateInfo.subresourceRange.aspectMask = _AspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = _MipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(vulkanDevice->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView))
	{
		Err() << "failed to create image view!" << std::endl;
	}

	return imageView;
}
