#pragma once
#include "Model.h"

class SphereModel : public Model
{
public:
	SphereModel(float _Radius, unsigned int _Rings, unsigned int _Sectors, Texture* _Texture = nullptr, Shader* _ShaderProgram = nullptr);
	virtual ~SphereModel();
	virtual void CreateModel() override;
	virtual void CreateModel(float _Radius, unsigned int _Rings, unsigned int _Sectors);
};