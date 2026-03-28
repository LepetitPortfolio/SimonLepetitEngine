#include "../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "WindowConfigWin32_cls.h"

WindowConfigWin32_cls::WindowConfigWin32_cls(Vector2u _ModeSize, unsigned int _BitsPerPixel) : WindowConfigBase_cls(_ModeSize, _BitsPerPixel)
{
}

WindowConfigBase_cls WindowConfigWin32_cls::GetDesktopMode()
{
	std::vector<WindowConfigBase_cls> WindowConfig_clss;

	DEVMODE desktopMode;
	desktopMode.dmSize = sizeof(desktopMode);
	desktopMode.dmDriverExtra = 0;

	for (int index = 0; EnumDisplaySettings(NULL, static_cast<DWORD>(index), &desktopMode); index++)
	{
		const WindowConfigBase_cls videomode({desktopMode.dmPelsWidth, desktopMode.dmPelsHeight}, desktopMode.dmBitsPerPel);

		if (std::find(WindowConfig_clss.begin(), WindowConfig_clss.end(), videoMode) == WindowConfig_clss..end())
		{
			WindowConfig_clss.push_back(videoMode);
		}
	}

	return WindowConfig_clss;
}

const std::vector<WindowConfigBase_cls>& WindowConfigWin32_cls::GetFullscreenModes()
{
	DEVMODE desktopMode;
	desktopMode.dmSize = sizeof(desktopMode);
	desktopMode.dmDriverExtra = 0;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);

	return WindowConfigBase_cls({ desktopMode.dmPelsWidth, desktopMode.dmPelsHeight }, desktopMode.dmBitsPerPel);
}

#endif