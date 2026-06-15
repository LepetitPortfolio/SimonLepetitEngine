#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include "..\..\GameplayConcepts\IDrawableGameObject.h"

#include "../../Common/Time.h"
#include "../Vertex.h"

#include "../Texture.h"

#include "../Shader.h"

#include <map>
#include <vector>

#define PI 3.14159265


class Model : public IDrawableGameObject
{
public:
	Model();
	Model(Texture* _Texture, Shader* _ShaderProgram = nullptr);
	virtual ~Model();

	std::vector<Vertex>& GetVertices();
	std::vector<uint32_t>& GetIndices();
	VkBuffer& GetVertexBuffer();
	VkDeviceMemory& GetVertexBufferMemory();
	VkBuffer& GetIndexBuffer();
	VkDeviceMemory& GetIndexBufferMemory();

	void ShowInGame(bool _Value);

	virtual void Draw(VulkanData& _VulkanData, VkCommandBuffer& _CommandBuffer, uint32_t _ImageIndex) override final;
	virtual void SetProgram(Shader* _ShaderProgram) override final;
	void Destroy() override final;

	virtual void SetTexture(Texture* _Texture)override final;
	virtual const Texture* GetTexture() const override final;

	void AddVertex(Vertex _Vertex);
	void AddIndex(uint32_t _Index);

protected:

	Time m_StartTime;

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;

	Texture* m_Texture = nullptr;

	Shader* m_ShaderProgram = nullptr;

	// --------------------------------------------------------------------
	// Liste des sets de descripteurs, un pour chaque frame en vol.
	// Chaque set contient les descripteurs (ex: buffer uniforme, texture) pour une frame donnée.
	std::vector<VkDescriptorSet> m_DescriptorSets;

	void UpdateDescriptorSets();
};






