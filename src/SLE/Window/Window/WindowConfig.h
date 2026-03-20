#pragma once
#include "../../Common/Vector.h"
#include <vector>

class WindowConfig
{
public:

	WindowConfig() = default;
	WindowConfig(Vector2u _ModeSize, unsigned int _BitsPerPixel);
	
	static WindowConfig GetDesktopMode();
	static const std::vector<WindowConfig>& GetFullscreenModes();

	bool IsValid() const;

	Vector2u m_Size;
	unsigned int m_BitsPerPixel = 32;

};