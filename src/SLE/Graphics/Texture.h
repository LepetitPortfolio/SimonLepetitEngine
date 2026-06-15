#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include "../Core/TextureInfos.h"

class Texture
{
public:
	Texture();
	Texture(TextureInfos& _TextureInfos);
	Texture(const char* _TexturePath);
	~Texture();

	inline TextureInfos& GetTextureInfos() { return m_TextureInfos; }
	inline VkImage& GetTextureImage() { return m_TextureImage; }
	inline VkDeviceMemory& GetTextureMemory() { return m_TextureMemory; }
	inline VkImageView& GetTextureImageView() { return m_TextureImageView; }
	inline VkSampler& GetTextureSampler() { return m_TextureSampler; }

	void CleanupTexture();

private:
	TextureInfos m_TextureInfos;
	VkImage m_TextureImage;
	VkDeviceMemory m_TextureMemory;
	VkImageView m_TextureImageView;
	VkSampler m_TextureSampler;
};