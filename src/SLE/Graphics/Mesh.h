#pragma once
#include "Models/Model.h"
#include "Shader.h"
#include "Textures/Texture.h"
#include "../Core/Transform.h"
#include "../GameplayConcepts/CameraBase.h"
#include "../System/VulkanStructs.h"

class Mesh 
{
public:
	Mesh(Model* _Model, Shader* _Shader, Texture* _Texture);

	~Mesh();

	VkBuffer GetUniformBuffer(size_t _FrameIndex) const { return m_UniformBuffers[_FrameIndex]; }
	std::vector<VkBuffer> GetUniformBuffers() const { return m_UniformBuffers; }
	std::vector<void*>& GetUniformBuffersMapped() { return m_UniformBuffersMapped; }

	void Draw(Transform* _Transform, VulkanFrameInfo& _FrameInfo);
	void Cleanup();

	

protected:

	Model* m_Model = nullptr;
	Shader* m_Shader = nullptr;
	Texture* m_Texture = nullptr;

	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMemory;
	std::vector<void*> m_UniformBuffersMapped;
	
	// --------------------------------------------------------------------
	// Liste des sets de descripteurs, un pour chaque frame en vol.
	// Chaque set contient les descripteurs (ex: buffer uniforme, texture) pour une frame donnée.
	std::vector<VkDescriptorSet> m_DescriptorSets;

	// --------------------------------------------------------------------
	// Pool de descripteurs : réservoir de mémoire pour allouer des sets de descripteurs.
	// Les sets de descripteurs lient des ressources (buffers, textures) à des bindings dans les shaders.
	VkDescriptorPool m_DescriptorPool;

	void UpdateDescriptorSets();

	void CreateUniformBuffers();

	void CreateDescriptorPool();

	void UpdateUniforms(Transform* _Transform, CameraBase* _Camera, uint32_t _ImageIndex);

};