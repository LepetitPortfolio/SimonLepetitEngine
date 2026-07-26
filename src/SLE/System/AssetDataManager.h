#pragma once
#include "AssetDataManagerBase.h"

#include <memory>


class AssetDataManager
{
public:
	AssetDataManager();
	~AssetDataManager();

	AssetDataManagerBase* GetShaderManager() { return m_ShaderManager.get(); }
	AssetDataManagerBase* GetTextureManager() { return m_TextureManager.get(); }
	AssetDataManagerBase* GetModelManager() { return m_ModelManager.get(); }
	AssetDataManagerBase* GetGameObjectManager() { return m_GameObjectManager.get(); }

	void AddData(class Shader* _Data);
	void AddData(class Texture* _Data);
	void AddData(class Model* _Data);
	void AddData(class GameObjectBase* _Data);
	void RemoveData(class Shader* _Data);
	void RemoveData(class Texture* _Data);
	void RemoveData(class Model* _Data);
	void RemoveData(class GameObjectBase* _Data);

	void ClearAllShaderData();
	void ClearAllTextureData();
	void ClearAllModelData();
	void ClearAllGameObjectData();
	void ClearAllData();


protected:
	std::unique_ptr <AssetDataManagerBase> m_ShaderManager;
	std::unique_ptr <AssetDataManagerBase> m_TextureManager;
	std::unique_ptr <AssetDataManagerBase> m_ModelManager;
	std::unique_ptr <AssetDataManagerBase> m_GameObjectManager;

};