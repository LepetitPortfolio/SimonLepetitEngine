#pragma once
#include "Models/Model.h"

class ModelLoader
{
public:

	/**
	* Charge un modèle 3D depuis un fichier OBJ et retourne un pointeur vers un nouvel objet Model.
	* 1. Crée une nouvelle instance de Model.
	* 2. Charge les données du fichier OBJ avec LoadModelFile.
	* 3. Crée les buffers Vulkan pour les vertices et les indices avec CreateVertexBuffer et CreateIndexBuffer.
	* _FilePath : Chemin vers le fichier OBJ à charger (ex: "models/model.obj").
	*/
	static Model* LoadModel(const char* _FilePath, class Texture* _Texture = nullptr, class Shader* _Shader = nullptr);

private :

	/**
	* Charge un fichier OBJ et remplit un objet Model avec ses vertices et indices.
	* Utilise la bibliothèque tinyobjloader pour parser le fichier OBJ.
	* Pour chaque forme (shape) et chaque indice dans le maillage :
	* 1. Extrait les coordonnées de position (x, y, z) depuis attrib.vertices.
	* 2. Extrait les coordonnées de texture (u, v) depuis attrib.texcoords (si disponibles) et inverse la coordonnée v.
	* 3. Définit une couleur par défaut (blanc) pour le vertex.
	* 4. Utilise une map (uniqueVertices) pour éviter les doublons de vertices :
	*    - Si le vertex n'existe pas encore, l'ajoute à la liste des vertices du modèle et stocke son index.
	*    - Ajoute l'index du vertex (unique) à la liste des indices du modèle.
	* _FilePath : Chemin vers le fichier OBJ à charger.
	* _Model : Référence vers l'objet Model à remplir avec les vertices et indices.
	*/
	static void LoadModelFile(const char* _FilePath, Model* _Model);

	/**
	* Crée un buffer de vertices (VkBuffer) pour un modèle 3D et le remplit avec les données des vertices.
	* Utilise une approche en deux étapes (staging buffer) pour transférer les données du CPU vers le GPU :
	* 1. Crée un staging buffer (mémoire accessible par le CPU) et copie les données des vertices dedans.
	* 2. Crée le vertex buffer final (mémoire locale au GPU, optimisée pour les performances).
	* 3. Copie les données du staging buffer vers le vertex buffer via VulkanPlatform::CopyBuffer.
	* 4. Libère le staging buffer et sa mémoire.
	* _Model : Référence vers l'objet Model contenant les vertices à transférer vers le GPU.
	*/
	static void CreateVertexBuffer(Model* _Model);

	/**
	* Crée un buffer d'indices (VkBuffer) pour un modèle 3D et le remplit avec les indices du modèle.
	* Utilise la même approche en deux étapes que CreateVertexBuffer :
	* 1. Crée un staging buffer pour transférer les indices du CPU vers le GPU.
	* 2. Copie les données des indices dans le staging buffer.
	* 3. Crée le index buffer final (mémoire locale au GPU, optimisée pour les performances).
	* 4. Copie les données du staging buffer vers le index buffer via VulkanPlatform::CopyBuffer.
	* 5. Libère le staging buffer et sa mémoire.
	* _Model : Référence vers l'objet Model contenant les indices à transférer vers le GPU.
	*/
	static void CreateIndexBuffer(Model* _Model);
};