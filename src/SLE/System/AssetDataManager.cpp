#include "AssetDataManager.h"

AssetDataManager::AssetDataManager()
{
	m_ShaderManager = std::make_unique<DataManagerBase<Shader*>>();
	m_TextureManager = std::make_unique<DataManagerBase<Texture*>>();
	m_ModelManager = std::make_unique<DataManagerBase<Model*>>();
}

AssetDataManager::~AssetDataManager()
{
	m_ShaderManager.reset();
	m_TextureManager.reset();
	m_ModelManager.reset();
}

void AssetDataManager::AddData(Shader* _Data)
{
	m_ShaderManager->AddData(_Data);
}

void AssetDataManager::AddData(Texture* _Data)
{
	m_TextureManager->AddData(_Data);
}

void AssetDataManager::AddData(Model* _Data)
{
	m_ModelManager->AddData(_Data);
}

void AssetDataManager::RemoveData(Shader* _Data)
{
	m_ShaderManager->RemoveData(_Data);
}

void AssetDataManager::RemoveData(Texture* _Data)
{
	m_TextureManager->RemoveData(_Data);
}

void AssetDataManager::RemoveData(Model* _Data)
{
	m_ModelManager->RemoveData(_Data);
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

void AssetDataManager::ClearAllData()
{
	ClearAllShaderData();
	ClearAllTextureData();
	ClearAllModelData();
}