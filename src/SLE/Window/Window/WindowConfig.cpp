#include "WindowConfig.h"

#include <algorithm>
#include <functional>

WindowConfig::WindowConfig(Vector2u _ModeSize, unsigned int _BitsPerPixel)
{
	m_Size = _ModeSize;
	m_BitsPerPixel = _BitsPerPixel;
}

bool WindowConfig::IsValid() const
{
	const std::vector<WindowConfig>& windowConfigs = GetFullscreenModes();

	return std::find(windowConfigs.begin(), windowConfigs.end(), *this) != windowConfigs.end();
}