#pragma once
#include "../../Platform.h"
#include "Cursor.h"

#include "WindowEnums.h"

#include "../../Common/Vector.h"

#include <memory>
#include <optional>

#include <cstdint>

class CursorData
{
public:

	CursorData(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);


	CursorData(const CursorData& _Cursor) = delete;

	~CursorData();

	CursorData& operator=(const CursorData& _Cursor) = delete;

	virtual bool LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot) = 0;

	virtual bool loadFromSystem(CursorType _CursorType) = 0;

private:

	virtual void Release() = 0;

};