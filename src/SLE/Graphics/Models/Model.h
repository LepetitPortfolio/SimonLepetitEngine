#pragma once
#include "..\..\GameplayConcepts\IDrawableGameObject.h"
#include "../../Core/AssetData.h"

#include "../../System/VulkanIncludes.h"
#include "../../Common/Time.h"
#include "../Vertex.h"

#include "../Texture.h"

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

	std::vector<Vertex>& GetVertices();
	std::vector<uint32_t>& GetIndices();

	virtual void CreateModel() override;
	void CreateModel(const std::string& _Filepath);

	virtual void Draw(VkCommandBuffer _CommandBuffer) override final;

	void Bind(VkCommandBuffer _CommandBuffer);


	//virtual void SetProgram(Shader* _ShaderProgram) override final;
	void Destroy() override final;

	//virtual void SetTexture(Texture* _Texture)override final;
	//virtual const Texture* GetTexture() const override final;

	void AddVertex(Vertex _Vertex);
	void AddIndex(uint32_t _Index);

protected:

	Time m_StartTime;

	std::vector<Vertex> m_Vertices;
	std::unique_ptr<VulkanBuffer> m_VertexBuffer;
	uint32_t m_VertexCount;

	std::vector<uint32_t> m_Indices;
	std::unique_ptr<VulkanBuffer> m_IndexBuffer;
	uint32_t m_IndexCount;

	bool m_HasIndexBuffer = false;


	// --------------------------------------------------------------------
	// Liste des sets de descripteurs, un pour chaque frame en vol.
	// Chaque set contient les descripteurs (ex: buffer uniforme, texture) pour une frame donnée.
	std::vector<VkDescriptorSet> m_DescriptorSets;

	void CreateVertexBuffers();
	void CreateIndexBuffers();


private:

	void LoadModel(const std::string& _Filepath);

};






