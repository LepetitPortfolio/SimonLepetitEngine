#pragma once
#include "DataManagerBase.h"

#include <memory>


class AssetDataManager
{
public:
	AssetDataManager();
	~AssetDataManager();

	DataManagerBase<class Shader*>* GetShaderManager() { return m_ShaderManager.get(); }
	DataManagerBase<class Texture*>* GetTextureManager() { return m_TextureManager.get(); }
	DataManagerBase<class Model*>* GetModelManager() { return m_ModelManager.get(); }


protected:
	std::unique_ptr <DataManagerBase<class Shader*>> m_ShaderManager;
	std::unique_ptr <DataManagerBase<class Texture*>> m_TextureManager;
	std::unique_ptr <DataManagerBase<class Model*>> m_ModelManager;

};