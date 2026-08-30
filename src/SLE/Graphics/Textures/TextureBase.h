#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include "../../Core/AssetData.h"

#include <string>


class TextureBase : public AssetData
{
public:
	TextureBase();

	TextureBase(const TextureBase&) = delete;
	TextureBase& operator=(const TextureBase&) = delete;

	virtual ~TextureBase() override;

	inline VkImage GetTextureImage() { return m_TextureImage; }
	inline VkDeviceMemory GetTextureMemory() { return m_TextureMemory; }
	inline VkImageView GetTextureImageView() { return m_TextureImageView; }

	void CleanupTextureBase();

protected :
	std::string m_Name;
	uint32_t m_MipLevels = 1;
	VkExtent2D m_Size{};
	VkImage m_TextureImage = VK_NULL_HANDLE;
	VkDeviceMemory m_TextureMemory = VK_NULL_HANDLE;
	VkImageView m_TextureImageView = VK_NULL_HANDLE;

	/**
	* Crée une vue d'image (VkImageView) pour une texture.
	* La vue permet d'accéder à l'image dans les shaders (ex: dans un fragment shader pour l'échantillonnage).
	* Utilise le format RGBA8 SRGB et inclut tous les niveaux de mipmap de la texture.
	* _Texture : Référence vers l'objet TextureBase dont on veut créer la vue.
	*/
	void CreateTextureImageView();

	/**
	* Génère les niveaux de mipmap pour une image texture.
	* Utilise une commande ponctuelle (single-time command) pour :
	* 1. Transitionner chaque niveau de mipmap vers TRANSFER_SRC_OPTIMAL après avoir été rempli.
	* 2. Effectuer un "blit" (copie avec réduction) du niveau actuel vers le niveau suivant, en divisant les dimensions par 2.
	* 3. Transitionner le niveau vers SHADER_READ_ONLY_OPTIMAL une fois généré.
	* Vérifie au préalable que le format de l'image supporte le filtrage linéaire (nécessaire pour le blit).
	* _Image : Image Vulkan pour laquelle générer les mipmaps.
	* _ImageFormat : Format de l'image (ex: VK_FORMAT_R8G8B8A8_SRGB).
	* _TexWidth, _TexHeight : Dimensions initiales (en pixels) de l'image.
	* _MipLevels : Nombre total de niveaux de mipmap à générer.
	*/
	void GenerateMipMaps(VkImage _Image, VkFormat _ImageFormat, int32_t _TexWidth, int32_t _TexHeight, uint32_t _MipLevels);

	/**
	* Transitionne le layout d'une image Vulkan entre deux états.
	* Utilise une barrière de pipeline (VkImageMemoryBarrier) pour synchroniser l'accès à l'image.
	* Prend en charge deux transitions spécifiques :
	* - UNDEFINED -> TRANSFER_DST_OPTIMAL : Prépare l'image à recevoir des données (ex: depuis un buffer).
	* - TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL : Prépare l'image à être lue par les shaders.
	* Pour toute autre transition, affiche une erreur.
	* _Image : Image Vulkan dont on veut changer le layout.
	* _Format : Format de l'image.
	* _OldLayout : Layout actuel de l'image.
	* _NewLayout : Nouveau layout souhaité.
	* _MipLevels : Nombre de niveaux de mipmap concernés par la transition.
	*/
	void TransitionImageLayout(VkImage _Image, VkFormat _Format, VkImageLayout _OldLayout, VkImageLayout _NewLayout, uint32_t _MipLevels);

	bool HasStencilComponent(VkFormat _Format);

	/**
	* Copie les données d'un buffer vers une image Vulkan.
	* Utilise une commande ponctuelle (single-time command) pour effectuer la copie.
	* L'image doit être dans le layout TRANSFER_DST_OPTIMAL pour que la copie soit valide.
	* _Buffer : Buffer source contenant les données à copier (ex: un buffer de staging).
	* _Image : Image destination.
	* _Width, _Height : Dimensions (en pixels) de la région à copier dans l'image.
	*/
	void CopyBufferToImage(VkBuffer _Buffer, VkImage _Image, uint32_t _Width, uint32_t _Height);

	void CreateImage(uint32_t _Width, uint32_t _Height, uint32_t _MipLevels, VkSampleCountFlagBits _NumSamples, VkFormat _Format, VkImageTiling _Tiling, VkImageUsageFlags _Usage, VkMemoryPropertyFlags _Properties, VkImage& _Image, VkDeviceMemory& _ImageMemory);

	VkImageView CreateImageView(VkImage _Image, VkFormat _Format, VkImageAspectFlags _AspectFlags, uint32_t _MipLevels);
};