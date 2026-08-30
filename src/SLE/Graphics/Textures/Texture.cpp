#include "Texture.h"

#include "../../Core/GlobalFunctionLibrary.h"
#include "../../System/AssetDataManager.h"
#include "../../System/VulkanIncludes.h"
#include "../../Common/Error.h"


#include <stb_image.h>


Texture::Texture()
{
	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

Texture::Texture(const char* _TexturePath)
{
	LoadTexture(_TexturePath);
	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);

}

Texture::~Texture()
{
	CleanupTexture();
}

void Texture::CleanupTexture()
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	vkDestroySampler(vulkanDevice->GetLogicalDevice(), m_TextureSampler, nullptr);

	CleanupTextureBase();
}

void Texture::LoadTexture(const char* _FilePath)
{
	CreateTextureImage(_FilePath);
	CreateTextureSampler();
	CreateTextureImageView();

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

void Texture::CreateTextureImage(const char* _FilePath)
{
	int texChannels;
	int width, height;

	stbi_uc* pixels = stbi_load(_FilePath, &width, &height, &texChannels, STBI_rgb_alpha);

	m_Size.height = static_cast<uint32_t>(height);
	m_Size.width = static_cast<uint32_t>(width);

	VkDeviceSize imageSize = m_Size.width * m_Size.height * 4;
	m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_Size.width, m_Size.height)))) + 1;

	if (!pixels)
	{
		Err() << "failed to load texture image!" << std::endl;
	}

	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanBufferManager* bufferManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetBufferManager();

	bufferManager->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vulkanDevice->GetLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(vulkanDevice->GetLogicalDevice(), stagingBufferMemory);

	stbi_image_free(pixels);

	CreateImage(m_Size.width, m_Size.height, m_MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureMemory);

	TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
	CopyBufferToImage(stagingBuffer, m_TextureImage, m_Size.width, m_Size.height);
	vkDestroyBuffer(vulkanDevice->GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(vulkanDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);

	GenerateMipMaps(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, m_Size.width, m_Size.height, m_MipLevels);
}

void Texture::CreateTextureSampler()
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(vulkanDevice->GetPhysicalDevice(), &properties);

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

	if (vkCreateSampler(vulkanDevice->GetLogicalDevice(), &samplerCreateInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
	{
		Err() << "failed to create texture sampler!" << std::endl;
	}
}
