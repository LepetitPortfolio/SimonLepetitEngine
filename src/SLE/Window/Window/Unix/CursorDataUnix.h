#pragma once
#include "../../../Platform.h"

#if PLATFORM_LINUX

#include "../CursorData.h"
#include "../Cursor.h"

#include "Display.h"
#include "../WindowEnums.h"

#include "../../../Common/Vector.h"

#include <memory>
#include <optional>

#include <cstdint>

class CursorDataUnix : public CursorData
{
public:

	CursorData();


	virtual bool LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot) override;

	virtual bool loadFromSystem(CursorType _CursorType) override;

private:

	friend class WindowUnix;

	std::shared_ptr<::Display> m_Display;
	Cursor m_Cursor{ WindowStyle::None };

	bool IsColorCursorSupported();

	bool LoadFromPixelsARGB(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);

	bool LoadFromPixelsMonochrome(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);


	virtual void Release() = 0;

};

#endif
