#pragma once
#define GLFW_INCLUDE_VULKAN
#include "Textures/Texture.h"

class TextureLoader
{
public : 

	/**
	* Charge une texture depuis un fichier et retourne un pointeur vers une nouvelle instance de Texture.
	* Alloue dynamiquement une Texture, puis appelle LoadTexture pour la remplir avec les données du fichier.
	* _FilePath : Chemin vers le fichier d'image à charger (ex: "textures/image.png").
	*/
	static Texture* LoadTexture(const char* _FilePath);

private:

	
};