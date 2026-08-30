#include "TextureVoid.h"

#include "../../Core/GlobalFunctionLibrary.h"
#include "../../System/AssetDataManager.h"
#include "../../System/VulkanIncludes.h"



TextureVoid::TextureVoid()
{
	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

TextureVoid::~TextureVoid()
{
	CleanupTextureVoid();
}

void TextureVoid::GenerateDepthResources()
{
	// Recreate safely when the swapchain extent changes. The TextureVoid object
	// itself remains owned by AssetDataManager.
	if (m_TextureImage != VK_NULL_HANDLE || m_TextureImageView != VK_NULL_HANDLE || m_TextureMemory != VK_NULL_HANDLE)
	{
		CleanupTextureBase();
	}

	VulkanSwapchain* vulkanSwapchain = GlobalFunctionLibrary::GetVulkanPlatform()->GetVulkanSwapchain();
    VkFormat depthFormat = FindDepthFormat();
    m_MipLevels = 1;
	m_Size = vulkanSwapchain->GetExtent();
    
    CreateImage(m_Size.width, m_Size.height, m_MipLevels , VK_SAMPLE_COUNT_1_BIT, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureMemory);
    
    m_TextureImageView = CreateImageView(m_TextureImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, m_MipLevels);
    
    TransitionImageLayout(m_TextureImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, m_MipLevels);
}

void TextureVoid::CleanupTextureVoid()
{
	CleanupTextureBase();
}


VkFormat TextureVoid::FindDepthFormat() 
{
    VulkanRenderer* vulkanRenderer = GlobalFunctionLibrary::GetVulkanPlatform()->GetVulkanRenderer();

    return vulkanRenderer->FindSupportedFormat( { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
}