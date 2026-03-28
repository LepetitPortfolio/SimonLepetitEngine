#include "WindowConfig_cls.h"

#include <algorithm>
#include <functional>

WindowConfigBase_cls::WindowConfigBase_cls(Vector2u _ModeSize, unsigned int _BitsPerPixel)
{
	m_Size = _ModeSize;
	m_BitsPerPixel = _BitsPerPixel;
}

bool WindowConfigBase_cls::IsValid() const
{
	const std::vector<WindowConfigBase_cls>& WindowConfig_clss = GetFullscreenModes();

	return std::find(WindowConfig_clss.begin(), WindowConfig_clss.end(), *this) != WindowConfig_clss.end();
}