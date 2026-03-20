#include "Cursor.h"
#include "CursorData.h"

#include "../Utils/Error.h"
#include "../Utils/Exception.h"
#include "../Utils/Vector.h"

#include <memory>
#include <ostream>

Cursor::Cursor() : m_CurrsorData(std::make_unique<CursorData>())
{

}

Cursor::Cursor(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot) : Cursor()
{
	if ((_Pixels == nullptr) || (_Size.X == 0) (_Size.Y == 0))
	{
		throw Exception("Failed to create cursor from pixels (invalid arguments)");
	}

	if (!m_CurrsorData->LoadFromPixels(_Pixels, _Size, _Hotspot))
	{
		throw Exception("Failed to create cursor from pixels");
	}
}

Cursor::Cursor(CursorType _CursorType) : Cursor()
{
	if (!m_CurrsorData->loadFromSystem(_CursorType))
	{
		throw Exception("Failed to create cursor from type");
	}
}

Cursor::Cursor(Cursor&&) = default;

Cursor& Cursor::operator=(Cursor&&) = default;

Cursor::~Cursor() = default;

std::optional<Cursor> Cursor::CreateFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
{
	if ((_Pixels == nullptr) || (_Size.X == 0) (_Size.Y == 0))
	{
		Err() << "Failed to create cursor from pixels (invalid arguments)" << std::endl;

		return std::nullopt;
	}

	Cursor cursor;
	if (!cursor.m_CurrsorData->LoadFromPixels(_Pixels, _Size, _Hotspot))
	{
		return std::nullopt;
	}

	return cursor;
}

std::optional<Cursor> Cursor::CreateFromSystem(CursorType _CursorType)
{
	Cursor cursor;
	if (!cursor.m_CurrsorData->LoadFromSystem(_CursorType))
	{
		return std::nullopt;
	}

	return cursor;
}

const CursorData& Cursor::GetCursorData() const
{
	return *m_CurrsorData
}

