#pragma once
#include "../../DataConfig.h"

#if PLATFORM_WINDOWS

#include "../WindowConfig_cls.h"

#include "../../Common/Vector.h"
#include <vector>

class WindowConfigWin32_cls : public WindowConfigBase_cls
{
public:

	WindowConfigWin32_cls() = default;
	WindowConfigWin32_cls(Vector2u _ModeSize, unsigned int _BitsPerPixel);
	
	static WindowConfigBase_cls GetDesktopMode();
	static const std::vector<WindowConfigBase_cls>& GetFullscreenModes();

};

#endif