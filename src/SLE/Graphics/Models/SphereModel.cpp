#include "SphereModel.h"
#include "../../Core/GlobalFunctionLibrary.h"
#include "../../System/AssetDataManager.h"



SphereModel::SphereModel(float _Radius, unsigned int _Rings, unsigned int _Sectors)
{
	CreateModel(_Radius, _Rings, _Sectors);

	CreateVertexBuffers();
	CreateIndexBuffers();

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

SphereModel::~SphereModel()
{
}

void SphereModel::CreateModel()
{
	CreateModel(1.0f, 16, 16);

	CreateVertexBuffers();
	CreateIndexBuffers();

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

void SphereModel::CreateModel(float _Radius, unsigned int _Rings, unsigned int _Sectors)
{
	const float ringsRecip = 1.0 / (float)(_Rings - 1);
	const float sectorsRecip = 1.0 / (float)(_Sectors - 1);
	int countRings;
	int countSectors;

	for (countRings = 0; countRings < _Rings; countRings++)
	{
		const float y = sin(-PI / 2 + PI * countRings * ringsRecip) * _Radius;
		for (countSectors = 0; countSectors < _Sectors; countSectors++)
		{
			const float x = cos(2 * PI * countSectors * sectorsRecip) * sin(PI * countRings * ringsRecip);
			const float z = sin(2 * PI * countSectors * sectorsRecip) * sin(PI * countRings * ringsRecip);

			Vertex vertex{};
			vertex.Position = glm::vec3(x * _Radius, y, z * _Radius);
			vertex.UV = glm::vec2(countSectors * sectorsRecip, countRings * ringsRecip);
			vertex.Color = glm::vec3(1.0f, 1.0f, 1.0f);

			m_Vertices.push_back(vertex);
		}
	}

	m_Indices.resize(_Rings * _Sectors * 6);
	std::vector<uint32_t>::iterator i = m_Indices.begin();
	for (countRings = 0; countRings < _Rings - 1; countRings++)
	{
		for (countSectors = 0; countSectors < _Sectors - 1; countSectors++)
		{
			*i++ = (countRings + 0) * _Sectors + countSectors;
			*i++ = (countRings + 0) * _Sectors + (countSectors + 1);
			*i++ = (countRings + 1) * _Sectors + (countSectors + 1);
			*i++ = (countRings + 0) * _Sectors + countSectors;
			*i++ = (countRings + 1) * _Sectors + countSectors;
			*i++ = (countRings + 1) * _Sectors + (countSectors + 1);
		}
	}

	//CreateVertexBuffer();
	//CreateIndexBuffer();
}
