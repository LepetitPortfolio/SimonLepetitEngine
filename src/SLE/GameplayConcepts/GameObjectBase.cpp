#include "GameObjectBase.h"

GameObjectBase::GameObjectBase()
{
}

void GameObjectBase::AddComponent(GameObjectComponentBase* _Component)
{
	uint64_t id = reinterpret_cast<uint64_t>(_Component);

	if(m_Components.count(id) == 0)
	{
		_Component->Init();
		_Component->Enabled(true);
		m_Components[id] = _Component;
	}	
}

void GameObjectBase::RemoveComponent(GameObjectComponentBase* _Component)
{
	uint64_t id = reinterpret_cast<uint64_t>(_Component);

	if (m_Components.count(id) == 0)
	{
		GameObjectComponentBase* component =  m_Components[id];
		component->Enabled(false);
		m_Components.erase(id);
		delete component;
	}
}

void GameObjectBase::ClearComponents()
{
	for (auto component : m_Components)
	{
		component.second->Enabled(false);
		delete component.second;
	}
	m_Components.clear();
}

void GameObjectBase::UpdateComponents()
{
	for (auto component : m_Components)
	{
		component.second->Update();
	}
}
