#pragma once
#include "InputsGlobal.h"

#include "../Utils/Vector.h"

enum class MouseButton : InputID
{
	Unknown = -1,
	Left = 0,
	Right,
	Middle,
	Button1,
	Button2
};

static constexpr unsigned int MouseButtonCount = static_cast<unsigned int>(MouseButton::Button2) + 1;

enum class MouseWheel : InputID
{
	Unknown = -1,
	Vertical = 0,
	Horizontal
};

