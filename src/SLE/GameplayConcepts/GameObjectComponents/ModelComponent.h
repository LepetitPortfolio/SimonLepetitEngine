#pragma once
#include "../GameObjectComponentBase.h"
#include "../../Graphics/Models/Model.h"

class ModelComponent : public GameObjectComponentBase
{
public:
	ModelComponent(Model* _Model);
	~ModelComponent();

	Model* GetModel() { return m_Model; }
	void SetModel(Model* _Model);

	virtual void Init() override;
	virtual void Desinit() override;
	virtual void Update() override;

protected:
	Model* m_Model;
};