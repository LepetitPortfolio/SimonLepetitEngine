#include "ModelLoader.h"
#include "../Common/Error.h"
#include "../Core/GlobalFunctionLibrary.h"

#include "../System/AssetDataManager.h"
#include "../System/DataManagerBase.h"
#include "../System/VulkanPlatform.h"

#include "Vertex.h"

#include <tiny_obj_loader.h>
#include <unordered_map>


Model* ModelLoader::LoadModel(const char* _FilePath, Texture* _Texture, Shader* _Shader)
{
	Model* model = new Model();

	LoadModelFile(_FilePath, model);
	CreateVertexBuffer(model);
	CreateIndexBuffer(model);

	if (_Texture)
	{
		model->SetTexture(_Texture);
	}
	if (_Shader)
	{
		model->SetProgram(_Shader);
	}

	GlobalFunctionLibrary::GetAssetDataManager()->GetModelManager()->AddData(model);


	return model;
}

void ModelLoader::LoadModelFile(const char* _FilePath, Model* _Model)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	std::string warn;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _FilePath))
	{
		Err() << err << std::endl;
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};

			vertex.Position =
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (attrib.texcoords.size() >= 2)
			{
				vertex.TexCoord =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			vertex.Color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(_Model->GetVertices().size());
				_Model->AddVertex(vertex);
			}

			_Model->AddIndex(uniqueVertices[vertex]);
		}
	}
}

void ModelLoader::CreateVertexBuffer(Model* _Model)
{
	VkDeviceSize bufferSize = sizeof(_Model->GetVertices()[0]) * _Model->GetVertices().size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, _Model->GetVertices().data(), (size_t)bufferSize);
	vkUnmapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory);

	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _Model->GetVertexBuffer(), _Model->GetVertexBufferMemory());

	VulkanPlatform::CopyBuffer(stagingBuffer, _Model->GetVertexBuffer(), bufferSize);

	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, nullptr);
}

void ModelLoader::CreateIndexBuffer(Model* _Model)
{
	VkDeviceSize bufferSize = sizeof(_Model->GetIndices()[0]) * _Model->GetIndices().size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, _Model->GetIndices().data(), (size_t)bufferSize);
	vkUnmapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory);
	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _Model->GetIndexBuffer(), _Model->GetIndexBufferMemory());

	VulkanPlatform::CopyBuffer(stagingBuffer, _Model->GetIndexBuffer(), bufferSize);	
	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, nullptr);
}