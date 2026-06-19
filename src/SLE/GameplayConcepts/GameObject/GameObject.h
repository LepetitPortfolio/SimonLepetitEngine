#pragma once
#include "../GameObjectBase.h"

class GameObject : public GameObjectBase
{
public:

	GameObject();
	~GameObject();

	virtual void FirstUpdate() override;
	virtual void Update() override;
	virtual void LastUpdate() override;

};