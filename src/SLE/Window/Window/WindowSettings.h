#pragma once

#include <cstdint>

struct WindowSettings
{
	enum Attribute
	{
		Default = 0,
		Core = 1 << 0,
		Debug = 1 << 2
	};

	unsigned int DepthBits{};
	unsigned int StencilBits{};
	unsigned int AntialiasingLevel{};
	unsigned int MajorVersion{1};
	unsigned int MinorVersion{1};
	unsigned int AttributeFlags{ Attribute::Default };
	bool SRGBCapable{};
};
