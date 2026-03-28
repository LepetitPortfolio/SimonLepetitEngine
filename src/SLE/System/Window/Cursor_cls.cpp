#include "Cursor_cls.h"

#include "CursorPlatform.h"

#include "../../Common/Error.h"
#include "../../Common/Exception.h"
#include "../../Common/Vector.h"

#include <memory>
#include <ostream>

Cursor_cls::Cursor_cls() : m_CursorPlatform(std::make_unique<CursorPlatform>())
{

}

Cursor_cls::Cursor_cls(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot) : Cursor_cls()
{
	if ((_Pixels == nullptr) || (_Size.X == 0) || (_Size.Y == 0))
	{
		throw Exception("Failed to create Cursor_cls from pixels (invalid arguments)");
	}

	if (!m_CursorPlatform->LoadFromPixels(_Pixels, _Size, _Hotspot))
	{
		throw Exception("Failed to create Cursor_cls from pixels");
	}
}

Cursor_cls::Cursor_cls(CursorType_e _CursorType) : Cursor_cls()
{
	if (!m_CursorPlatform->LoadFromSystem(_CursorType))
	{
		throw Exception("Failed to create Cursor_cls from type");
	}
}

std::optional<Cursor_cls> Cursor_cls::CreateFromPixels(const std::uint8_t *_Pixels, Vector2u _Size, Vector2u _Hotspot)
{
    if ((_Pixels == nullptr) || (_Size.X == 0) || (_Size.Y == 0))
	{
		Err() << "Failed to create Cursor_cls from pixels (invalid arguments)" << std::endl;

		return std::nullopt;
	}

	Cursor_cls cursor;
	if (!cursor.m_CursorPlatform->LoadFromPixels(_Pixels, _Size, _Hotspot))
	{
		return std::nullopt;
	}

	return cursor;
}

std::optional<Cursor_cls> Cursor_cls::CreateFromSystem(CursorType_e _CursorType)
{
	Cursor_cls cursor;
	if (!cursor.m_CursorPlatform->LoadFromSystem(_CursorType))
	{
		return std::nullopt;
	}

	return cursor;
}

const CursorPlatform& Cursor_cls::GetCursorPlatform() const
{
	return *m_CursorPlatform;
}

