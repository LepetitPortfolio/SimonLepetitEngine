#pragma once
#include "TextureBase.h"


class TextureVoid : public TextureBase
{
	public:
	TextureVoid();
	~TextureVoid() override;
	TextureVoid(const TextureVoid&) = delete;
	TextureVoid& operator=(const TextureVoid&) = delete;
	void GenerateDepthResources();

	void CleanupTextureVoid();

protected:

	VkFormat FindDepthFormat();
};