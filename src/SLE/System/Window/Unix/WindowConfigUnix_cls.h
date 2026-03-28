#pragma once
#include "../../DataConfig.h"

#if PLATFORM_LINUX

#include "../WindowConfig_cls.h"

#include "../../Common/Vector.h"
#include <vector>

class WindowConfigUnix_cls : public WindowConfigBase_cls
{
public:

	WindowConfigUnix_cls() = default;
	WindowConfigUnix_cls(Vector2u _ModeSize, unsigned int _BitsPerPixel);
	
	static WindowConfigBase_cls GetDesktopMode();
	static const std::vector<WindowConfigBase_cls>& GetFullscreenModes();
};

#endif