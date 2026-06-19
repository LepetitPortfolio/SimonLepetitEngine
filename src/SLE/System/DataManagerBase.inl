#include "DataManagerBase.h"

template<typename T>
inline DataManagerBase<T>::DataManagerBase()
{
}

template<typename T>
inline DataManagerBase<T>::~DataManagerBase()
{
	if (!m_DataList.empty())
	{
		ClearAllData();
	}
}

template<typename T>
inline void DataManagerBase<T>::AddData(T _Data)
{
	if (_Data == nullptr)
	{
		return;
	}

	uint64_t id = reinterpret_cast<uint64_t>(_Data);

	if (m_DataList.count(id) == 0)
	{
		m_DataList[id] = _Data;
	}
}

template<typename T>
inline void DataManagerBase<T>::RemoveData(T _Data)
{
	if (_Data == nullptr)
	{
		return;
	}

	uint64_t id = reinterpret_cast<uint64_t>(_Data);

	if (m_DataList.count(id) != 0)
	{
		T data = m_DataList[id];
		m_DataList.erase(id);
		delete data;
	}
}

template<typename T>
inline void DataManagerBase<T>::ClearAllData()
{
	for (auto data : m_DataList)
	{
		delete data.second;
	}
	m_DataList.clear();
}
