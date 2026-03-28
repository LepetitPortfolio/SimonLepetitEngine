#pragma once
#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "../CursorPlatform.h"
#include "../Cursor_cls.h"

#include "../WindowEnums.h"

#include ".../../../Common/Vector.h"

#include <memory>
#include <optional>

#include <cstdint>

class CursorWin32_cls
{
public:


	CursorWin32_cls() = default;

	bool LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);

	bool loadFromSystem(CursorType_e _CursorType);

private:

	friend class WindowWin32_cls;


	void* m_Cursor{};
	bool m_SystemCursor_cls{};


	void Release();

};

#endif
