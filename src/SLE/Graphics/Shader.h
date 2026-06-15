#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include <string>

class Shader
{
public:
	Shader();
	Shader(std::string _ShaderName);
	~Shader();

	std::string GetShaderName() const;
	VkDescriptorSetLayout& GetDescriptorSetLayout();
	VkPipelineLayout& GetPipelineLayout();
	VkPipeline& GetPipeline();

	void Cleanup();

private :

	std::string m_ShaderName;

	// --------------------------------------------------------------------
	// Layout des sets de descripteurs : définit la structure des ressources accessibles dans les shaders.
	// Spécifie les bindings (ex: buffer uniforme en binding 0, texture en binding 1) et leurs types.
	VkDescriptorSetLayout m_DescriptorSetLayout;

	// --------------------------------------------------------------------
	// Layout du pipeline graphique : définit les ressources (descripteurs, push constants) utilisées par le pipeline.
	// Contient les layouts des sets de descripteurs et les plages de push constants.
	VkPipelineLayout m_PipelineLayout;

	// --------------------------------------------------------------------
	// Pipeline graphique : définit toutes les étapes fixes du rendu (shaders, assemblage des primitives, rasterization, etc.).
	// Représente le "chemin" que suivent les données pour être transformées en pixels à l'écran.
	VkPipeline m_Pipeline;

};
