#include "TextureLoader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../Engine.h"
#include "../System/VulkanPlatform.h"
#include "../Common/Error.h"
#include <stb_image.h>


Texture* TextureLoader::LoadTexture(const char* _FilePath)
{
	Texture* texture = new Texture();
	LoadTexture(_FilePath, *texture);

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(texture);

	return texture;
}

void TextureLoader::LoadTexture(const char* _FilePath, Texture& _Texture)
{
	if (!Engine::IsInstanced())
	{
		Err() << "Engine is not instanced!" << std::endl;
		return;
	}

	CreateTextureImage(_FilePath, _Texture);
	CreateTextureSampler(_Texture);
	CreateTextureImageView(_Texture);
}

void TextureLoader::CreateTextureImage(const char* _FilePath, Texture& _Texture)
{
	int texChannels;

	TextureInfos& textureInfos = _Texture.GetTextureInfos();

	stbi_uc* pixels = stbi_load(_FilePath, &textureInfos.Size.X, &textureInfos.Size.Y, &texChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = textureInfos.Size.X * textureInfos.Size.Y * 4;
	textureInfos.MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureInfos.Size.X, textureInfos.Size.Y)))) + 1;

	if (!pixels)
	{
		Err() << "failed to load texture image!" << std::endl;
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VulkanPlatform::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory);

	stbi_image_free(pixels);

	VulkanPlatform::CreateImage(textureInfos.Size.X, textureInfos.Size.Y, textureInfos.MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _Texture.GetTextureImage(), _Texture.GetTextureMemory());

	TransitionImageLayout(_Texture.GetTextureImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _Texture.GetTextureInfos().MipLevels);
	CopyBufferToImage(stagingBuffer, _Texture.GetTextureImage(), static_cast<uint32_t>(textureInfos.Size.X), static_cast<uint32_t>(textureInfos.Size.Y));

	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, nullptr);

	GenerateMipMaps(_Texture.GetTextureImage(), VK_FORMAT_R8G8B8A8_SRGB, textureInfos.Size.X, textureInfos.Size.Y, textureInfos.MipLevels);
}

void TextureLoader::CreateTextureImageView(Texture& _Texture)
{
	_Texture.GetTextureImageView() = VulkanPlatform::CreateImageView(_Texture.GetTextureImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, _Texture.GetTextureInfos().MipLevels);
}

void TextureLoader::CreateTextureSampler(Texture& _Texture)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(GlobalFunctionLibrary::GetVulkanData()->PhysicalDevice, &properties);

	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = VK_LOD_CLAMP_NONE;
	samplerCreateInfo.mipLodBias = 0.0f;

	if (vkCreateSampler(GlobalFunctionLibrary::GetVulkanDevice(), &samplerCreateInfo, nullptr, &_Texture.GetTextureSampler()) != VK_SUCCESS)
	{
		Err() << "failed to create texture sampler!" << std::endl;
	}
}

void TextureLoader::GenerateMipMaps(VkImage _Image, VkFormat _ImageFormat, int32_t _TexWidth, int32_t _TexHeight, uint32_t _MipLevels)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(GlobalFunctionLibrary::GetVulkanData()->PhysicalDevice, _ImageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		Err() << "texture image format does not support linear blitting!" << std::endl;
	}

	VkCommandBuffer commandBuffer = VulkanPlatform::BeginSingleTimeCommands();

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

	VulkanPlatform::EndSingleTimeCommands(commandBuffer);
}

void TextureLoader::TransitionImageLayout(VkImage _Image, VkFormat _Format, VkImageLayout _OldLayout, VkImageLayout _NewLayout, uint32_t _MipLevels)
{
	VkCommandBuffer commandBuffer = VulkanPlatform::BeginSingleTimeCommands();

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

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

	if ((_OldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (_NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
	{
		// Transition pour préparer l'image à recevoir des données
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if ((_OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (_NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
	{
		// Transition pour préparer l'image à être lue par les shaders
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		Err() << "unsupported layout transition!" << std::endl;
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	VulkanPlatform::EndSingleTimeCommands(commandBuffer);
}

void TextureLoader::CopyBufferToImage(VkBuffer _Buffer, VkImage _Image, uint32_t _Width, uint32_t _Height)
{
	VkCommandBuffer commandBuffer = VulkanPlatform::BeginSingleTimeCommands();

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

	VulkanPlatform::EndSingleTimeCommands(commandBuffer);
}