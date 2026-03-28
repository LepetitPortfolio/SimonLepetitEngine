#pragma once
#include "../../DataConfig.h"

#include "../../Common/Vector.h"

#include <cstdint>
#include <memory>
#include <optional>


class CursorPlatform;

class  Cursor_cls
{
	
public:	
	
enum class CursorType_e
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

	Cursor_cls(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);
	Cursor_cls(CursorType_e _CursorType);
	Cursor_cls(const Cursor_cls&) = delete;
	Cursor_cls(Cursor_cls&&) noexcept = default;

	~Cursor_cls() = default;

	Cursor_cls& operator=(const Cursor_cls&) = delete;
	Cursor_cls& operator=(Cursor_cls&&) noexcept = default;

	static std::optional<Cursor_cls> CreateFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot);

	static std::optional<Cursor_cls> CreateFromSystem(CursorType_e _CursorType);

private:
	friend class WindowBase_cls;

	Cursor_cls();

	const CursorPlatform& GetCursorPlatform() const;

	std::unique_ptr<CursorPlatform> m_CursorPlatform;
};