#include "Texture.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "TextureLoader.h"

Texture::Texture()
{
}

Texture::Texture(TextureInfos& _TextureInfos)
{
	TextureLoader::LoadTexture(_TextureInfos.Name.c_str(), *this);
}

Texture::Texture(const char* _TexturePath)
{
	TextureLoader::LoadTexture(_TexturePath, *this);
}

Texture::~Texture()
{
	CleanupTexture();
}


void Texture::CleanupTexture()
{
	vkDestroySampler(GlobalFunctionLibrary::GetVulkanDevice(), m_TextureSampler, nullptr);
	vkDestroyImageView(GlobalFunctionLibrary::GetVulkanDevice(), m_TextureImageView, nullptr);

	vkDestroyImage(GlobalFunctionLibrary::GetVulkanDevice(), m_TextureImage, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_TextureMemory, nullptr);
}
