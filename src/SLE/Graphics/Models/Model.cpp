#include "Model.h"
#include "../../Core/GlobalFunctionLibrary.h"
#include "../../System/VulkanPlatform.h"
#include "../../Common/Error.h"
#include "../../System/AssetDataManager.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <functional>



Model::Model()
{
	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

Model::Model(const std::string& _Filepath)
{
	CreateModel(_Filepath);
	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

Model::~Model()
{
	m_Vertices.clear();
	m_Indices.clear();

	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
}

std::vector<Vertex>& Model::GetVertices()
{
	return m_Vertices;
}

std::vector<uint32_t>& Model::GetIndices()
{
	return m_Indices;
}


void Model::CreateModel()
{
}

void Model::CreateModel(const std::string& _Filepath)
{
	LoadModel(_Filepath);
	CreateVertexBuffers();
	CreateIndexBuffers();
}

void Model::Draw(VkCommandBuffer _CommandBuffer)
{
	if (m_HasIndexBuffer) 
	{
		vkCmdDrawIndexed(_CommandBuffer, m_IndexCount, 1, 0, 0, 0);
	}
	else 
	{
		vkCmdDraw(_CommandBuffer, m_VertexCount, 1, 0, 0);
	}
}

void Model::Bind(VkCommandBuffer _CommandBuffer)
{
	VkBuffer buffers[] = { m_VertexBuffer->GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(_CommandBuffer, 0, 1, buffers, offsets);

	if (m_HasIndexBuffer) 
	{
		vkCmdBindIndexBuffer(_CommandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}

void Model::Destroy()
{
	m_Vertices.clear();
	m_Indices.clear();

	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
	
}

void Model::AddVertex(Vertex _Vertex)
{
	m_Vertices.push_back(_Vertex);
}

void Model::AddIndex(uint32_t _Index)
{
	m_Indices.push_back(_Index);
}


void Model::CreateVertexBuffers()
{
	VulkanPlatform* vulkanPlatform = GlobalFunctionLibrary::GetVulkanPlatform();

	m_VertexCount = static_cast<uint32_t>(m_Vertices.size());
	assert(m_VertexCount >= 3 && "Vertex count must be at least 3");

	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_VertexCount;
	uint32_t vertexSize = sizeof(m_Vertices[0]);

	VulkanBuffer stagingBuffer{ vertexSize, m_VertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,};

	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void*)m_Vertices.data(), bufferSize);

	m_VertexBuffer = std::make_unique<VulkanBuffer>(vertexSize, m_VertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vulkanPlatform->CopyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
}

void Model::CreateIndexBuffers()
{
	m_IndexCount = static_cast<uint32_t>(m_Indices.size());
	m_HasIndexBuffer = m_IndexCount > 0;

	if (!m_HasIndexBuffer) {
		return;
	}

	VulkanPlatform* vulkanPlatform = GlobalFunctionLibrary::GetVulkanPlatform();

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_IndexCount;
	uint32_t indexSize = sizeof(m_Indices[0]);

	VulkanBuffer stagingBuffer(indexSize, m_IndexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void*)m_Indices.data(), bufferSize);

	m_IndexBuffer = std::make_unique<VulkanBuffer>(indexSize, m_IndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vulkanPlatform->CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
}

/*
void Model::UpdateDescriptorSets()
{
	if(m_ShaderProgram == nullptr)
	{
		return;
	}

	for (size_t frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; frameIndex++)
	{
		std::vector<VkDescriptorSetLayoutBinding>* layoutBinding = m_ShaderProgram->GetLayoutBinding();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets{};

		for (size_t layoutBindingIndex = 0; layoutBindingIndex < layoutBinding->size(); layoutBindingIndex++)
		{
			VkDescriptorSetLayoutBinding lBinding = layoutBinding->at(layoutBindingIndex);
			
			switch (lBinding.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

				m_ShaderProgram->GenertateUniformBufferDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], frameIndex, writeDescriptorSets);
				
			break;

			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:

				m_ShaderProgram->GenertateCombinedImageSamplerDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], m_Texture, writeDescriptorSets);
			break;

			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:

				//m_ShaderProgram->GenertateStorageBufferDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], writeDescriptorSets);
				
			break;

			};
		}

		vkUpdateDescriptorSets(GlobalFunctionLibrary::GetVulkanDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}
*/

void Model::LoadModel(const std::string& _Filepath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _Filepath.c_str())) 
	{
		throw std::runtime_error(warn + err);
	}

	m_Vertices.clear();
	m_Indices.clear();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			if (index.vertex_index >= 0) 
			{
				vertex.Position = 
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};

				vertex.Color = 
				{
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2],
				};
			}

			if (index.normal_index >= 0) 
			{
				vertex.Normal = 
				{
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
				};
			}

			if (index.texcoord_index >= 0) 
			{
				vertex.UV = 
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
				m_Vertices.push_back(vertex);
			}
			m_Indices.push_back(uniqueVertices[vertex]);
		}
	}
}
