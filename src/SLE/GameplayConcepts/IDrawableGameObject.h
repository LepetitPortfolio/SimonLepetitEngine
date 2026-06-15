#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "..\System\VulkanStructs.h"
#include "..\Core\TextureInfos.h"


#include <vector>
#include <iostream>

class IDrawableGameObject
{
public:
	virtual ~IDrawableGameObject() = 0;

	virtual void Draw(VulkanData& _VulkanData, VkCommandBuffer& _CommandBuffer, uint32_t _ImageIndex) = 0;
	virtual void SetProgram(class Shader* _ShaderProgram) = 0;
	virtual void Destroy() = 0;

	virtual void SetTexture(class Texture* _Texture) = 0;
	virtual const class Texture* GetTexture() const = 0;
	//virtual const uint32_t GetTexture(int _NumTexture) const = 0;

};

inline IDrawableGameObject::~IDrawableGameObject() {}