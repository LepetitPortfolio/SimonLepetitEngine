#pragma once
#include "../System/AssetDataManagerBase.h"

#include <cstdint>

class AssetData
{
public:
	AssetData() = default;
	~AssetData() = default;

	AssetData(const AssetData&) = delete;
	AssetData& operator=(const AssetData&) = delete;

	uint64_t GetUID() { return m_UID; };

protected:

	uint64_t m_UID = 0;

	friend class AssetDataManagerBase;
};