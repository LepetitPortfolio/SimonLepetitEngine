#pragma once
#include "Model.h"

class SphereModel : public Model
{
public:
	SphereModel(float _Radius, unsigned int _Rings, unsigned int _Sectors);
	virtual ~SphereModel();
	virtual void CreateModel() override;
	virtual void CreateModel(float _Radius, unsigned int _Rings, unsigned int _Sectors);
};