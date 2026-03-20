#include <System/Platform.h>
#include "VideoMode.h"

#if PLATFORM_WINDOWS

WindowConfig WindowConfig::GetDesktopMode()
{
	std::vector<WindowConfig> windowConfigs;

	DEVMODE desktopMode;
	desktopMode.dmSize = sizeof(desktopMode);
	desktopMode.dmDriverExtra = 0;

	for (int index = 0; EnumDisplaySettings(NULL, static_cast<DWORD>(index), &desktopMode); index++)
	{
		const WindowConfig videomode({desktopMode.dmPelsWidth, desktopMode.dmPelsHeight}, desktopMode.dmBitsPerPel);

		if (std::find(windowConfigs.begin(), windowConfigs.end(), videoMode) == windowConfigs..end())
		{
			windowConfigs.push_back(videoMode);
		}
	}

	return windowConfigs;
}

const std::vector<WindowConfig>& WindowConfig::GetFullscreenModes()
{
	DEVMODE desktopMode;
	desktopMode.dmSize = sizeof(desktopMode);
	desktopMode.dmDriverExtra = 0;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);

	return WindowConfig({ desktopMode.dmPelsWidth, desktopMode.dmPelsHeight }, desktopMode.dmBitsPerPel);
}

#endif