#include "LocationComponent.h"
#include "../GameObjectBase.h"


LocationComponent::LocationComponent(Transform _LocalTransform)
{
	m_LocalTransform = _LocalTransform;
}

LocationComponent::~LocationComponent()
{
	m_GameObjectParent = nullptr;
}


void LocationComponent::Init(GameObjectBase* _GameObjectParent)
{
	if (_GameObjectParent)
	{
		m_GameObjectParent = _GameObjectParent;
		m_WorldTransform = m_LocalTransform + m_GameObjectParent->GetTransform();
	}
}

void LocationComponent::Desinit()
{
}

void LocationComponent::Update()
{
	m_WorldTransform = m_LocalTransform;
	if (m_GameObjectParent)
	{
		m_WorldTransform += *m_GameObjectParent->GetTransform();
	}
}

void LocationComponent::EnabledAction()
{
}

void LocationComponent::DisableAction()
{
}
