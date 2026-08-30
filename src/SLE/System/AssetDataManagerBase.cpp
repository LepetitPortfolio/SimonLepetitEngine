#include "AssetDataManagerBase.h"
#include "../Core/AssetData.h"

AssetDataManagerBase::AssetDataManagerBase()
{
}

AssetDataManagerBase::~AssetDataManagerBase()
{
	if (!m_DataList.empty())
	{
		ClearAllData();
	}
}

void AssetDataManagerBase::AddData(AssetData* _Data)
{
	if (_Data == nullptr)
	{
		return;
	}

	AssetData* assetData = static_cast<AssetData*>(_Data);
	if (assetData)
	{  // Vérification redondante ici, car _Data != nullptr
		uint64_t id = reinterpret_cast<uint64_t>(_Data);
		assetData->m_UID = id;  // Utilise -> pour accéder à m_UID

		if (m_DataList.count(id) == 0)
		{
			m_DataList[id] = _Data;
		}
	}

}

void AssetDataManagerBase::RemoveData(AssetData* _Data)
{
	if (_Data == nullptr)
	{
		return;
	}
	
	AssetData* assetData = static_cast<AssetData*>(_Data);
	if (assetData)
	{
		uint64_t id = assetData->m_UID;
		if (m_DataList.count(id) != 0)
		{
			AssetData* data = m_DataList[id];
			m_DataList.erase(id);
			delete data;
		}
	}
	
}

void AssetDataManagerBase::ClearAllData()
{
	for (auto data : m_DataList)
	{
		delete data.second;
	}
	m_DataList.clear();
}
