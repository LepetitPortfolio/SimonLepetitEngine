#include "TextureLoader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../Engine.h"
#include "../System/VulkanPlatform.h"
#include "../Common/Error.h"


Texture* TextureLoader::LoadTexture(const char* _FilePath)
{
	//GlobalFunctionLibrary::GetAssetDataManager()->AddData(texture);

	return new Texture(_FilePath);
}


