#pragma once
#include "../GameObjectComponentBase.h"
#include "../CameraBase.h"

class CameraComponent
{
public:
	CameraComponent(CameraBase* _CameraBase);
	~CameraComponent();


	virtual void Init() override;
	virtual void Desinit() override;
	virtual void Update() override;

protected:
	CameraBase* m_Camera;
};