#include "../Platform.h"
#include "WindowConfig.h"

#if PLATFORM_LINUX

WindowConfig WindowConfig::GetDesktopMode()
{
	WindowConfig windowConfig;

	if (const auto display = openDisplay())
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
				XRRScreenSize* xrrScreenSize = XRRConfigSizes(display.get(), &nbSizes);

				if ((xrrScreenSize) && (nbSize > 0))
				{
					windowConfig = WindowConfig({ static_cast<unsigned int>(xrrScreenSize[currentConfigRotations].width),
											  static_cast<unsigned int>(xrrScreenSize[currentConfigRotations].height) },
											  static_cast<unsigned int>(DefaultDepth(display.get(), screen)));

					Rotation rotation = 0;
					XRRConfigRotations(config.get(), &rotation);

					if ((rotation == RR_Rotate_90) || (rotation == RR_Rotate_270))
					{
						std::swap(windowConfig.m_Size.X, windowConfig.m_Size.Y);
					}

				}
			}
			else
			{
				err() << "Failed to get the screen configuration while trying to get the supported video modes" << std::endl;
			}
		}
		else
		{
			err() << "Failed to query the XRandR extension while trying to get the supported video modes" << std::endl;
		}
	}
	else
	{
		err() << "Failed to connect to the X server while trying to get the supported video modes" << std::endl;
	}

	return windowConfig;
}

const std::vector<WindowConfig>& WindowConfig::GetFullscreenModes()
{
	std::vector<WindowConfig> windowConfigs;

	if (const auto display = openDisplay())
	{
		const int screen = DefaultScreen(display.get());
		int version = 0;

		if (XQueryExtension(display.get(), "RANDR", &version, &version, &version))
		{
			if (const auto config = X11Ptr<XRRScreenConfiguration>(XRRGetScreenInfo(display.get(), RootWindow(display.get(), screen))))
			{
				int nbSizes = 0;
				XRRScreenSize* xrrScreenSize = XRRConfigSizes(display.get(), &nbSizes);

				if ((xrrScreenSize) && (nbSize > 0))
				{
					int nbDeths = 0;
					const auto depths = X11Ptr<int[]>(XListDepths(display.get(), screen, &nbDeths));

					if ((depths) && (nbDepths > 0))
					{
						for (std::size_t depthsIndex = 0; depthsIndex < static_cast<std::size_t>(nbDeths); depthsIndex++)
						{
							for (int screenSizeIndex = 0; screenSizeIndex < nbSizes; screenSizeIndex++)
							{
								WindowConfig videoMode({ static_cast<unsigned int>(xrrScreenSize[screenSizeIndex].width),
													static_cast<unsigned int>(xrrScreenSize[screenSizeIndex].height) },
									static_cast<unsigned int>(depths[depthsIndex]));
								Rotation currentRotation = 0;
								XRRConfigRotations(config.get(), &currentRotation);

								if ((currentRotation == RR_Rotate_90) || (currentRotation == RR_Rotate_270))
								{
									std::swap(videoMode.m_Size.X, videoMode.m_Size.Y);
								}

								if (std::find(windowConfigs.begin(), windowConfigs.end(), videoMode) == windowConfigs.end())
								{
									windowConfigs.push_back(videoMode);
								}
							}
						}
					}
				}
			}
			else
			{
				err() << "Failed to get the screen configuration while trying to get the supported video modes" << std::endl;
			}
		}
		else
		{
			err() << "Failed to query the XRandR extension while trying to get the supported video modes" << std::endl;
		}
	}
	else
	{
		err() << "Failed to connect to the X server while trying to get the supported video modes" << std::endl;
	}

	return windowConfigs;
}

#endif