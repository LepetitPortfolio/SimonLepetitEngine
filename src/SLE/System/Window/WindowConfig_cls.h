#pragma once
#include "../../DataConfig.h"

#include "../../Common/Vector.h"
#include <vector>

class  WindowConfigBase_cls
{
public:

	WindowConfigBase_cls() = default;
	WindowConfigBase_cls(Vector2u _ModeSize, unsigned int _BitsPerPixel);
	
	static WindowConfigBase_cls GetDesktopMode();
	static const std::vector<WindowConfigBase_cls>& GetFullscreenModes();

	bool IsValid() const;

	Vector2u m_Size;
	unsigned int m_BitsPerPixel = 32;

};


#if PLATFORM_WINDOWS
#include "win/WindowConfigWin32_cls.h"
using WindowConfig = WindowConfigWin_cls; 
#elif PLATFORM_LINUX
#include "Unix/WindowConfigUnix_cls.h"
using WindowConfig = WindowConfigUnix_cls; 
#endif