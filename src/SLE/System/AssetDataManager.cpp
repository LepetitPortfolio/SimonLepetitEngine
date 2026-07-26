#include "AssetDataManager.h"

AssetDataManager::AssetDataManager()
{
	m_ShaderManager = std::make_unique<AssetDataManagerBase>();
	m_TextureManager = std::make_unique<AssetDataManagerBase>();
	m_ModelManager = std::make_unique<AssetDataManagerBase>();
	m_GameObjectManager = std::make_unique<AssetDataManagerBase>();
}

AssetDataManager::~AssetDataManager()
{
	m_ShaderManager.reset();
	m_TextureManager.reset();
	m_ModelManager.reset();
	m_GameObjectManager.reset();
}

void AssetDataManager::AddData(Shader* _Data)
{
	m_ShaderManager->AddData((AssetData*)_Data);
}

void AssetDataManager::AddData(Texture* _Data)
{
	m_TextureManager->AddData((AssetData*)_Data);
}

void AssetDataManager::AddData(Model* _Data)
{
	m_ModelManager->AddData((AssetData*)_Data);
}

void AssetDataManager::AddData(GameObjectBase* _Data)
{
	m_GameObjectManager->AddData((AssetData*)_Data);
}


void AssetDataManager::RemoveData(Shader* _Data)
{
	m_ShaderManager->RemoveData((AssetData*)_Data);
}

void AssetDataManager::RemoveData(Texture* _Data)
{
	m_TextureManager->RemoveData((AssetData*)_Data);
}

void AssetDataManager::RemoveData(Model* _Data)
{
	m_ModelManager->RemoveData((AssetData*)_Data);
}

void AssetDataManager::RemoveData(GameObjectBase* _Data)
{
	m_GameObjectManager->RemoveData((AssetData*)_Data);
}

void AssetDataManager::ClearAllShaderData()
{
	m_ShaderManager->ClearAllData();
}

void AssetDataManager::ClearAllTextureData()
{
	m_TextureManager->ClearAllData();
}

void AssetDataManager::ClearAllModelData()
{
	m_ModelManager->ClearAllData();
}

void AssetDataManager::ClearAllGameObjectData()
{
	m_GameObjectManager->ClearAllData();
}

void AssetDataManager::ClearAllData()
{
	ClearAllShaderData();
	ClearAllTextureData();
	ClearAllModelData();
}