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
	Destroy();
}

std::vector<StandardVertex>& Model::GetVertices()
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
	//CreateUniformBuffers();

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
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(_CommandBuffer, 0, 1, vertexBuffers, offsets);

	if (m_HasIndexBuffer) 
	{
		vkCmdBindIndexBuffer(_CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
}

void Model::Destroy()
{
	m_Vertices.clear();
	m_Indices.clear();

	VulkanBufferManager* bufferManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetBufferManager();
	bufferManager->DestroyBuffer(m_VertexBuffer, m_VertexBufferMemory);
	bufferManager->DestroyBuffer(m_IndexBuffer, m_IndexBufferMemory);
}

void Model::AddVertex(StandardVertex _Vertex)
{
	m_Vertices.push_back(_Vertex);
}

void Model::AddIndex(uint32_t _Index)
{
	m_Indices.push_back(_Index);
}

void Model::CreateVertexBuffers()
{
	VulkanBufferManager* bufferManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetBufferManager();

	bufferManager->CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory);

}

void Model::CreateIndexBuffers()
{
	VulkanBufferManager* bufferManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetBufferManager();

	bufferManager->CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);
}

/*/void Model::CreateUniformBuffers()
{
	VulkanBufferManager* bufferManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetBufferManager();
	uint32_t maxFramesInFlight = GlobalFunctionLibrary::GetConfig()->MaxFramesInFlight;
	
	bufferManager->CreateUniformBuffer(maxFramesInFlight, m_UniformBuffers, m_UniformBuffersMemory, m_UniformBuffersMapped);
}*/

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

	std::unordered_map<StandardVertex, uint32_t> uniqueVertices{};
	for (const auto& shape : shapes) 
	{
		for (const auto& index : shape.mesh.indices) 
		{
			StandardVertex vertex{};

			if (index.vertex_index >= 0) 
			{
				vertex.Position = 
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};

				// OBJ files do not necessarily contain per-vertex colors.
				// Use white as the default and only read the color array when it exists.
				vertex.Color = glm::vec3(1.0f);

				if (!attrib.colors.empty() &&
					3 * index.vertex_index + 2 < static_cast<int>(attrib.colors.size()))
				{
					vertex.Color =
					{
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}
			}

			if (index.texcoord_index >= 0) 
			{
				vertex.UV = 
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}

			if (uniqueVertices.count(vertex) == 0) 
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
				m_Vertices.push_back(vertex);
			}
			m_Indices.push_back(uniqueVertices[vertex]);
		}
	}

	m_IndexCount = static_cast<uint32_t>(m_Indices.size());
	m_VertexCount = static_cast<uint32_t>(m_Vertices.size());
}
