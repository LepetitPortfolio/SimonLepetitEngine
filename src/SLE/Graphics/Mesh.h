#pragma once
#include "Models/Model.h"
#include "Shader.h"
#include "Texture.h"

class Mesh 
{
public:
	Mesh(Model* _Model, Shader* _Shader, Texture* _Texture);

	void Draw(Transform* _Transform, VulkanFrameInfo _FrameInfo);

protected:

	Model* m_Model = nullptr;
	Shader* m_Shader = nullptr;
	Texture* m_Texture = nullptr;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	void UpdateDescriptorSets();

};