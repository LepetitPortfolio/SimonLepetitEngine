#include <System/PlatformConfig.h>
#include "VideoMode.h"

#if PLATFORM_WINDOWS

WindowConfig_cls WindowConfig_cls::GetDesktopMode()
{
	std::vector<WindowConfig_cls> WindowConfig_clss;

	DEVMODE desktopMode;
	desktopMode.dmSize = sizeof(desktopMode);
	desktopMode.dmDriverExtra = 0;

	for (int index = 0; EnumDisplaySettings(NULL, static_cast<DWORD>(index), &desktopMode); index++)
	{
		const WindowConfig_cls videomode({desktopMode.dmPelsWidth, desktopMode.dmPelsHeight}, desktopMode.dmBitsPerPel);

		if (std::find(WindowConfig_clss.begin(), WindowConfig_clss.end(), videoMode) == WindowConfig_clss..end())
		{
			WindowConfig_clss.push_back(videoMode);
		}
	}

	return WindowConfig_clss;
}

const std::vector<WindowConfig_cls>& WindowConfig_cls::GetFullscreenModes()
{
	DEVMODE desktopMode;
	desktopMode.dmSize = sizeof(desktopMode);
	desktopMode.dmDriverExtra = 0;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);

	return WindowConfig_cls({ desktopMode.dmPelsWidth, desktopMode.dmPelsHeight }, desktopMode.dmBitsPerPel);
}

#endif