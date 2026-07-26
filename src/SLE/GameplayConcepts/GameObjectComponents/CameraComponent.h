#pragma once
#include "LocationComponent.h"
#include "../CameraBase.h"

class CameraComponent : public LocationComponent
{
public:
	CameraComponent(CameraBase* _CameraBase, Transform _LocalTransform = Transform{});
	~CameraComponent();


	virtual void Init(GameObjectBase* _GameObjectParent) override;
	virtual void Desinit() override;

protected:
	CameraBase* m_Camera;

	virtual void EnabledAction() override;
	virtual void DisableAction() override;
};