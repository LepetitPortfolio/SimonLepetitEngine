#pragma once
#include "../../PlatformConfig.h"

#include "Joystick.h"
#include "../../Common/EnumArray.h"

struct JoystickData
{
	unsigned int ButtonCount{};
	EnumArray<JoystickAxis_e, bool, JoystickAxisCount> Axes{};
};

struct JoystickState
{
	bool Connected{};
	EnumArray<JoystickAxis_e, bool, JoystickAxisCount> Axes{};
	std::array<bool, JoystickButtonCount> Buttons{};
};

#if PLATFORM_WINDOWS

#include "Win/JoystickWin32_cls.h"
typedef CursorWin32 JoystickPlatform;

#elif PLATFORM_LINUX

#include"Unix/JoystickUnix_cls.h"
typedef JoystickUnix_cls JoystickPlatform;

#endif