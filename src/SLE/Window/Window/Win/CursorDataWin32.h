#pragma once
#include "../../../Platform.h"

#if PLATFORM_WINDOWS

#include "../CursorData.h"
#include "../Cursor.h"

#include "../WindowEnums.h"

#include ".../../../Common/Vector.h"

#include <memory>
#include <optional>

#include <cstdint>

class CursorDataWin32 : public CursorData
{
public:


	CursorDataWin32() = default;

	virtual bool LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot) override;

	virtual bool loadFromSystem(CursorType _CursorType) = override;

private:

	friend class WindowWin32;


	void* m_Cursor{};
	bool m_SystemCursor{};


	virtual void Release() override;

};

#endif
