#include "ModelLoader.h"


Model* ModelLoader::LoadModel(const char* _FilePath)
{
	Model* model = new Model(_FilePath);

	return model;
}

SphereModel* ModelLoader::LoadSphereModel(float _Radius, uint32_t _SectorCount, uint32_t _StackCount)
{
	return new SphereModel(_Radius, _SectorCount, _StackCount);
}

