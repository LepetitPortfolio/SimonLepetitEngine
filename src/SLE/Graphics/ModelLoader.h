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
	static Model* LoadModel(const char* _FilePath);

private :


};