#pragma once
#include "InputsGlobal.h"

#include "../../Common/Vector.h"

enum class MouseButton_e : InputID
{
	Unknown = -1,
	Left = 0,
	Right,
	Middle,
	Extra1,
	Extra2
};

static constexpr unsigned int MouseButtonCount = static_cast<unsigned int>(MouseButton_e::Extra2) + 1;

enum class MouseWheel_e : InputID
{
	Unknown = -1,
	Vertical = 0,
	Horizontal
};

