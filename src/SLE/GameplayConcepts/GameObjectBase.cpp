#include "GameObjectBase.h"

GameObjectBase::GameObjectBase()
{
}

GameObjectBase::~GameObjectBase()
{
	ClearComponents();
}


void GameObjectBase::AddComponent(GameObjectComponentBase* _Component)
{
	if (_Component == nullptr)
	{
		return;
	}

	uint64_t id = _Component->GetUID();

	if(m_Components.count(id) == 0)
	{
		_Component->Init(this);

		_Component->Enabled(true);
		m_Components[id] = _Component;
	}	
}

void GameObjectBase::RemoveComponent(GameObjectComponentBase* _Component)
{
	if (_Component == nullptr)
	{
		return;
	}

	uint64_t id = _Component->GetUID();

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

void GameObjectBase::DrawGameObject(VulkanFrameInfo& _FrameInfo)
{
	if (m_DrawCallback.IsValid())
	{
		m_DrawCallback.Execute(_FrameInfo);
	}
}

void GameObjectBase::AddDrawCallback(const std::function<void(VulkanFrameInfo&)>& _Callback)
{
	m_DrawCallback += _Callback;
}

void GameObjectBase::RemoveDrawCallback(const std::function<void(VulkanFrameInfo&)>& _Callback)
{
	m_DrawCallback -= _Callback;
}
