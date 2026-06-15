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
