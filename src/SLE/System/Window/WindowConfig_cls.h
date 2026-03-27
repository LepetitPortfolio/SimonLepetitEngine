#pragma once
#include "../../DataConfig.h"

#include "../../Common/Vector.h"
#include <vector>

class  WindowConfig_cls
{
public:

	WindowConfig_cls() = default;
	WindowConfig_cls(Vector2u _ModeSize, unsigned int _BitsPerPixel);
	
	static WindowConfig_cls GetDesktopMode();
	static const std::vector<WindowConfig_cls>& GetFullscreenModes();

	bool IsValid() const;

	Vector2u m_Size;
	unsigned int m_BitsPerPixel = 32;

};