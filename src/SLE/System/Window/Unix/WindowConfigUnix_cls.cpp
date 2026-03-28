#include "../PlatformConfig.h"

#if PLATFORM_LINUX

#include "WindowConfigUnix_cls.h"

#include "../../Common/Error.h"

#include "Display.h"

#include <algorithm>
#include <ostream>

WindowConfigUnix_cls::WindowConfigUnix_cls(Vector2u _ModeSize, unsigned int _BitsPerPixel) : WindowConfigBase_cls(_ModeSize, _BitsPerPixel)
{
}

WindowConfigBase_cls WindowConfigUnix_cls::GetDesktopMode()
{
	WindowConfigBase_cls WindowConfig;

	if (const auto display = OpenDisplay())
	{
		const int screen = DefaultScreen(display.get());
		int version = 0;

		if (XQueryExtension(display.get(), "RANDR", &version, &version, &version))
		{
			if (const auto config = X11Ptr<XRRScreenConfiguration>(XRRGetScreenInfo(display.get(), RootWindow(display.get(), screen))))
			{
				Rotation currentRotation = 0;
				const int currentConfigRotations = XRRConfigRotations(config.get(), &currentRotation);

				int nbSizes = 0;
				XRRScreenSize* xrrScreenSize = XRRConfigSizes(config.get(), &nbSizes);

				if ((xrrScreenSize) && (nbSizes > 0))
				{
					WindowConfig = WindowConfigBase_cls({ static_cast<unsigned int>(xrrScreenSize[currentConfigRotations].width),
											  static_cast<unsigned int>(xrrScreenSize[currentConfigRotations].height) },
											  static_cast<unsigned int>(DefaultDepth(display.get(), screen)));

					Rotation rotation = 0;
					XRRConfigRotations(config.get(), &rotation);

					if ((rotation == RR_Rotate_90) || (rotation == RR_Rotate_270))
					{
						std::swap(WindowConfig.m_Size.X, WindowConfig.m_Size.Y);
					}

				}
			}
			else
			{
				Err() << "Failed to get the screen configuration while trying to get the supported video modes" << std::endl;
			}
		}
		else
		{
			Err() << "Failed to query the XRandR extension while trying to get the supported video modes" << std::endl;
		}
	}
	else
	{
		Err() << "Failed to connect to the X server while trying to get the supported video modes" << std::endl;
	}

	return WindowConfig;
}

const std::vector<WindowConfigBase_cls>& WindowConfigUnix_cls::GetFullscreenModes()
{
	std::vector<WindowConfigBase_cls> WindowConfig_clss;

	if (const auto display = OpenDisplay())
	{
		const int screen = DefaultScreen(display.get());
		int version = 0;

		if (XQueryExtension(display.get(), "RANDR", &version, &version, &version))
		{
			if (const auto config = X11Ptr<XRRScreenConfiguration>(XRRGetScreenInfo(display.get(), RootWindow(display.get(), screen))))
			{
				int nbSizes = 0;
				XRRScreenSize* xrrScreenSize = XRRConfigSizes(config.get(), &nbSizes);

				if ((xrrScreenSize) && (nbSizes > 0))
				{
					int nbDepths = 0;
					const auto depths = X11Ptr<int[]>(XListDepths(display.get(), screen, &nbDepths));

					if ((depths) && (nbDepths > 0))
					{
						for (std::size_t depthsIndex = 0; depthsIndex < static_cast<std::size_t>(nbDepths); depthsIndex++)
						{
							for (int screenSizeIndex = 0; screenSizeIndex < nbSizes; screenSizeIndex++)
							{
								WindowConfigBase_cls windowConfig({ static_cast<unsigned int>(xrrScreenSize[screenSizeIndex].width),
													static_cast<unsigned int>(xrrScreenSize[screenSizeIndex].height) },
									static_cast<unsigned int>(depths[depthsIndex]));
								Rotation currentRotation = 0;
								XRRConfigRotations(config.get(), &currentRotation);

								if ((currentRotation == RR_Rotate_90) || (currentRotation == RR_Rotate_270))
								{
									std::swap(windowConfig.m_Size.X, windowConfig.m_Size.Y);
								}

								if (std::find(WindowConfig_clss.begin(), WindowConfig_clss.end(), windowConfig) == WindowConfig_clss.end())
								{
									WindowConfig_clss.push_back(windowConfig);
								}
							}
						}
					}
				}
			}
			else
			{
				Err() << "Failed to get the screen configuration while trying to get the supported video modes" << std::endl;
			}
		}
		else
		{
			Err() << "Failed to query the XRandR extension while trying to get the supported video modes" << std::endl;
		}
	}
	else
	{
		Err() << "Failed to connect to the X server while trying to get the supported video modes" << std::endl;
	}

	return WindowConfig_clss;
}

#endif


