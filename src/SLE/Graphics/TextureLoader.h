#pragma once
#define GLFW_INCLUDE_VULKAN
#include "Texture.h"

class TextureLoader
{
public : 

	/**
	* Charge une texture depuis un fichier et retourne un pointeur vers une nouvelle instance de Texture.
	* Alloue dynamiquement une Texture, puis appelle LoadTexture pour la remplir avec les données du fichier.
	* _FilePath : Chemin vers le fichier d'image à charger (ex: "textures/image.png").
	*/
	static Texture* LoadTexture(const char* _FilePath);

	/**
	* Charge une texture depuis un fichier dans une instance Texture existante.
	* Vérifie que le moteur (Engine) est instancié, puis appelle les méthodes pour créer :
	* 1. L'image de texture (CreateTextureImage)
	* 2. L'échantillonneur (CreateTextureSampler)
	* 3. La vue de texture (CreateTextureImageView)
	* _FilePath : Chemin vers le fichier d'image à charger.
	* _Texture : Référence vers l'objet Texture à remplir.
	*/
	static void LoadTexture(const char* _FilePath, Texture& _Texture);

private:

	/**
	* Crée une image de texture Vulkan à partir d'un fichier image.
	* Étapes :
	* 1. Charge les pixels de l'image avec stbi_load (force le format RGBA).
	* 2. Calcule le nombre de niveaux de mipmap en fonction des dimensions de l'image.
	* 3. Crée un buffer de staging (mémoire accessible par le CPU) pour transférer les pixels vers le GPU.
	* 4. Copie les pixels dans le buffer de staging via vkMapMemory.
	* 5. Crée l'image Vulkan finale avec les paramètres adaptés (taille, format, usage pour échantillonnage et transfert).
	* 6. Transitionne le layout de l'image de UNDEFINED à TRANSFER_DST_OPTIMAL pour permettre la copie.
	* 7. Copie les données du buffer de staging vers l'image Vulkan.
	* 8. Génère les mipmaps pour l'image.
	* 9. Libère le buffer de staging et sa mémoire.
	* _FilePath : Chemin vers le fichier d'image à charger.
	* _Texture : Référence vers l'objet Texture à remplir (stocke l'image, la mémoire, et les infos de texture).
	*/
	static void CreateTextureImage(const char* _FilePath, Texture& _Texture);

	/**
	* Crée une vue d'image (VkImageView) pour une texture.
	* La vue permet d'accéder à l'image dans les shaders (ex: dans un fragment shader pour l'échantillonnage).
	* Utilise le format RGBA8 SRGB et inclut tous les niveaux de mipmap de la texture.
	* _Texture : Référence vers l'objet Texture dont on veut créer la vue.
	*/
	static void CreateTextureImageView(Texture& _Texture);

	/**
	* Crée un échantillonneur (VkSampler) pour une texture.
	* Configure les paramètres suivants :
	* - Filtrage : Linéaire pour la magnification et la minification.
	* - Adressage : Répétition (REPEAT) sur les axes U, V, et W.
	* - Anisotropie : Activée avec la valeur maximale supportée par le GPU.
	* - Mode de mipmap : Linéaire.
	* - Couleur de bordure : Noir opaque.
	* - Plage de LOD : De 0.0 à VK_LOD_CLAMP_NONE (tous les niveaux de mipmap).
	* _Texture : Référence vers l'objet Texture dont on veut créer l'échantillonneur.
	*/
	static void CreateTextureSampler(Texture& _Texture);

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
	static void GenerateMipMaps(VkImage _Image, VkFormat _ImageFormat, int32_t _TexWidth, int32_t _TexHeight, uint32_t _MipLevels);

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
	static void TransitionImageLayout(VkImage _Image, VkFormat _Format, VkImageLayout _OldLayout, VkImageLayout _NewLayout, uint32_t _MipLevels);

	/**
	* Copie les données d'un buffer vers une image Vulkan.
	* Utilise une commande ponctuelle (single-time command) pour effectuer la copie.
	* L'image doit être dans le layout TRANSFER_DST_OPTIMAL pour que la copie soit valide.
	* _Buffer : Buffer source contenant les données à copier (ex: un buffer de staging).
	* _Image : Image destination.
	* _Width, _Height : Dimensions (en pixels) de la région à copier dans l'image.
	*/
	static void CopyBufferToImage(VkBuffer _Buffer, VkImage _Image, uint32_t _Width, uint32_t _Height);
};