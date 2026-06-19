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

	void AddData(class Shader* _Data);
	void AddData(class Texture* _Data);
	void AddData(class Model* _Data);
	void RemoveData(class Shader* _Data);
	void RemoveData(class Texture* _Data);
	void RemoveData(class Model* _Data);

	void ClearAllShaderData();
	void ClearAllTextureData();
	void ClearAllModelData();
	void ClearAllData();


protected:
	std::unique_ptr <DataManagerBase<class Shader*>> m_ShaderManager;
	std::unique_ptr <DataManagerBase<class Texture*>> m_TextureManager;
	std::unique_ptr <DataManagerBase<class Model*>> m_ModelManager;

};