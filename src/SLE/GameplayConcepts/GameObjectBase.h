#pragma once
#include "GameObjectComponentBase.h"

#include "../Core/Transform.h"

#include <unordered_map>



class GameObjectBase
{
public:
	GameObjectBase();
	virtual ~GameObjectBase();
	virtual void FirstUpdate() = 0;
	virtual void Update() = 0;
	virtual void LastUpdate() = 0;

	void AddComponent(GameObjectComponentBase* _Component);
	void RemoveComponent(GameObjectComponentBase* _Component);
	void ClearComponents();
	void UpdateComponents();

protected:

	Transform m_Transform{};
	std::unordered_map<uint64_t, GameObjectComponentBase*> m_Components;

	bool m_Enabled = true;
};