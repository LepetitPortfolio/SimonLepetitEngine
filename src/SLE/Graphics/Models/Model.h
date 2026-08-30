#pragma once
#include "..\..\GameplayConcepts\IDrawableGameObject.h"
#include "../../Core/AssetData.h"

#include "../../System/VulkanBufferManager.h"
#include "../../Common/Time.h"
#include "../../Common/Vertex.h"

#include "../Textures/Texture.h"

#include "../Shader.h"

#include <memory>
#include <vector>

#define PI 3.14159265


class Model : public AssetData, public IDrawableGameObject
{
public:
	Model();
	Model(const std::string& _Filepath);
	virtual ~Model() override;

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	std::vector<StandardVertex>& GetVertices();
	std::vector<uint32_t>& GetIndices();

	VkBuffer GetVertexBuffer() const { return m_VertexBuffer; }

	VkBuffer GetIndexBuffer() const { return m_IndexBuffer; }
	uint32_t GetIndexCount() const { return m_IndexCount; }	

	VkBuffer GetUniformBuffer(size_t _FrameIndex) const { return m_UniformBuffers[_FrameIndex]; }
	std::vector<VkBuffer> GetUniformBuffers() const { return m_UniformBuffers; }
	std::vector<void*>& GetUniformBuffersMapped() { return m_UniformBuffersMapped; }


	virtual void CreateModel() override;
	void CreateModel(const std::string& _Filepath);

	virtual void Draw(VkCommandBuffer _CommandBuffer) override final;

	void Bind(VkCommandBuffer _CommandBuffer);


	//virtual void SetProgram(Shader* _ShaderProgram) override final;
	void Destroy() override final;

	//virtual void SetTexture(Texture* _Texture)override final;
	//virtual const Texture* GetTexture() const override final;

	void AddVertex(StandardVertex _Vertex);
	void AddIndex(uint32_t _Index);

protected:

	Time m_StartTime;

	std::vector<StandardVertex> m_Vertices;
	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	uint32_t m_VertexCount;

	std::vector<uint32_t> m_Indices;
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;
	uint32_t m_IndexCount;

	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMemory;
	std::vector<void*> m_UniformBuffersMapped;

	bool m_HasIndexBuffer = false;


	void CreateVertexBuffers();
	void CreateIndexBuffers();
	void CreateUniformBuffers();

private:

	void LoadModel(const std::string& _Filepath);

};






