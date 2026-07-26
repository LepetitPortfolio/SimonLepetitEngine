#pragma once
#include "../GameObjectComponentBase.h"
#include "../../Core/Transform.h"

class LocationComponent : public GameObjectComponentBase
{
public:
	LocationComponent(Transform _LocalTransform = Transform{});
	~LocationComponent();

	Transform* GetLocalTransform() { return &m_LocalTransform; }
	Transform* GetWorldTransform() { return &m_WorldTransform; }


	virtual void Init(GameObjectBase* _GameObjectParent) override;
	virtual void Desinit() override;
	virtual void Update() override;

protected:

	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	virtual void EnabledAction() override;
	virtual void DisableAction() override;

};