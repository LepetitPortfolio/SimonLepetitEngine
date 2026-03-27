#include "../PlatformConfig.h"
#include "WindowConfig_cls.h"

#if PLATFORM_LINUX

WindowConfig_cls WindowConfig_cls::GetDesktopMode()
{
	WindowConfig_cls WindowConfig_cls;

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
				XRRScreenSize* xrrScreenSize = XRRConfigSizes(display.get(), &nbSizes);

				if ((xrrScreenSize) && (nbSize > 0))
				{
					WindowConfig_cls = WindowConfig_cls({ static_cast<unsigned int>(xrrScreenSize[currentConfigRotations].width),
											  static_cast<unsigned int>(xrrScreenSize[currentConfigRotations].height) },
											  static_cast<unsigned int>(DefaultDepth(display.get(), screen)));

					Rotation rotation = 0;
					XRRConfigRotations(config.get(), &rotation);

					if ((rotation == RR_Rotate_90) || (rotation == RR_Rotate_270))
					{
						std::swap(WindowConfig_cls.m_Size.X, WindowConfig_cls.m_Size.Y);
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

	return WindowConfig_cls;
}

const std::vector<WindowConfig_cls>& WindowConfig_cls::GetFullscreenModes()
{
	std::vector<WindowConfig_cls> WindowConfig_clss;

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
								WindowConfig_cls videoMode({ static_cast<unsigned int>(xrrScreenSize[screenSizeIndex].width),
													static_cast<unsigned int>(xrrScreenSize[screenSizeIndex].height) },
									static_cast<unsigned int>(depths[depthsIndex]));
								Rotation currentRotation = 0;
								XRRConfigRotations(config.get(), &currentRotation);

								if ((currentRotation == RR_Rotate_90) || (currentRotation == RR_Rotate_270))
								{
									std::swap(videoMode.m_Size.X, videoMode.m_Size.Y);
								}

								if (std::find(WindowConfig_clss.begin(), WindowConfig_clss.end(), videoMode) == WindowConfig_clss.end())
								{
									WindowConfig_clss.push_back(videoMode);
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

	return WindowConfig_clss;
}

#endif