#include "Cursor_cls.h"
#include "CursorPlatform.h"

#include "../../Common/Error.h"
#include "../../Common/Exception.h"
#include "../../Common/Vector.h"

#include <memory>
#include <ostream>

Cursor_cls::Cursor_cls() : m_Cursor_clsPlatform(std::make_unique<CursorPlatform>())
{

}

Cursor_cls::Cursor_cls(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot) : Cursor_cls()
{
	if ((_Pixels == nullptr) || (_Size.X == 0) || (_Size.Y == 0))
	{
		throw Exception("Failed to create Cursor_cls from pixels (invalid arguments)");
	}

	if (!m_Cursor_clsPlatform->LoadFromPixels(_Pixels, _Size, _Hotspot))
	{
		throw Exception("Failed to create Cursor_cls from pixels");
	}
}

Cursor_cls::Cursor_cls(CursorType_e _CursorType) : Cursor_cls()
{
	if (!m_Cursor_clsPlatform->LoadFromSystem(_CursorType))
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

	Cursor_cls Cursor_cls;
	if (!Cursor_cls.m_Cursor_clsPlatform->LoadFromPixels(_Pixels, _Size, _Hotspot))
	{
		return std::nullopt;
	}

	return Cursor_cls;
}

std::optional<Cursor_cls> Cursor_cls::CreateFromSystem(CursorType_e _CursorType)
{
	Cursor_cls Cursor_cls;
	if (!Cursor_cls.m_Cursor_clsPlatform->LoadFromSystem(_CursorType))
	{
		return std::nullopt;
	}

	return Cursor_cls;
}

const CursorPlatform& Cursor_cls::GetCursor_clsPlatform() const
{
	return *m_Cursor_clsPlatform;
}

