#pragma once
#include "../../../PlatformConfig.h"

#if PLATFORM_LINUX

#include "../Cursor_cls.h"
#include "../WindowEnums.h"

#include "../../../Common/Vector.h"


class CursorUnix_cls
{
public:

	CursorUnix_cls();
	
	~CursorUnix_cls();

	bool LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);

	bool LoadFromSystem(Cursor_cls::CursorType_e _CursorType);

private:

	friend class WindowUnix_cls;

	std::shared_ptr<Display> m_Display;
	Cursor m_Cursor{ None };

	bool IsColorCursor_clsSupported();

	bool LoadFromPixelsARGB(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);

	bool LoadFromPixelsMonochrome(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);


	void Release();

};

#endif
