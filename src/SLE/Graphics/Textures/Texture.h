#pragma once
#include "TextureBase.h"

class Texture : public TextureBase
{
public:
	Texture();
	Texture(const char* _TexturePath);

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	virtual ~Texture() override;

	inline VkSampler GetTextureSampler() { return m_TextureSampler; }
	
	void CleanupTexture();

protected:
	VkSampler m_TextureSampler;

	void LoadTexture(const char* _FilePath);

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
	void CreateTextureImage(const char* _FilePath);

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
	void CreateTextureSampler();

};