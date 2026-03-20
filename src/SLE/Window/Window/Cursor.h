#pragma once

#include "../../Common/Vector.h"

#include <memory>
#include <optional>

#include <cstdint>

class CursorData;

enum class CursorType
{
	Arrow,
	ArrowWait,
	Wait,
	Text,
	Hand,
	SizeHorizontal,
	SizeVertical,
	SizeTopLeftBottomRight,
	SizeBottomLeftTopRight,
	SizeLeft,
	SizeRight,
	SizeTop,
	SizeBottom,
	SizeTopLeft,
	SizeBottomRight,
	SizeBottomLeft,
	SizeTopRight,
	SizeAll,
	Cross,
	Help,
	NotAllowed
};

class Cursor
{	
	Cursor(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);

	Cursor(CursorType _CursorType);

	Cursor(const Cursor&) = delete;

	Cursor& operator=(const Cursor&) = delete;

	Cursor(Cursor&&);

	Cursor& operator=(Cursor&&);

	~Cursor();

	static std::optional<Cursor> CreateFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);

	static std::optional<Cursor> CreateFromSystem(CursorType _CursorType);

private:
	friend class WindowBase;

	Cursor();

	const CursorData& GetCursorData() const;

	std::unique_ptr<CursorData> m_CurrsorData;
};