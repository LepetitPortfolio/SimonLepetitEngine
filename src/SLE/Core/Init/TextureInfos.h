#pragma once

#include "../Common/Vector.h"

#include <string>

struct TextureInfos
{
public:
	std::string Name;
	std::string ShaderVariableName;
	uint32_t MipLevels;
	Vector2i Size;
};
