#include "ModelLoader.h"
#include "../Common/Error.h"
#include "../Core/GlobalFunctionLibrary.h"

#include "../System/AssetDataManager.h"
#include "../System/AssetDataManagerBase.h"
#include "../System/VulkanPlatform.h"

#include "Vertex.h"

#include <tiny_obj_loader.h>
#include <unordered_map>


Model* ModelLoader::LoadModel(const char* _FilePath)
{
	Model* model = new Model(_FilePath);

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(model);

	return model;
}

