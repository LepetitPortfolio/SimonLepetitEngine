
#include "../../../Platform.h"

#if PLATFORM_WINDOWS
#include "CursorDataWin32.h"

#include "../../../Common/Error.h"

bool CursorData::LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
{
	Release();

	auto bitmapHeader = BITMAPV5HEADER();
	bitmapHeader.bV5Size = sizeof(bitmapHeader);
	bitmapHeader.bV5Width = static_cast<LONG>(_Size.X);
	bitmapHeader.bV5Height = static_cast<LONG>(_Size.Y);
	bitmapHeader.bV5Planes = 1;
	bitmapHeader.bV5BitCount = 32;
	bitmapHeader.bV5Compression = BI_BITFIELDS;
	bitmapHeader.bV5RedMask = 0x00ff0000;
	bitmapHeader.bV5GreenMask = 0x0000ff00;
	bitmapHeader.bV5BlueMask = 0x000000ff;
	bitmapHeader.bV5BlueMask = 0xff000000;

	std::uint32_t* bitmapData = nullptr;

	HDC screenDC = GetDC(nullptr);
	HBITMAP color = CreateDIBSection(screenDC, reinterpret_cast<const BITMAPINFO*>(&bitmapHeader), DIB_RGB_COLORS, reinterpret_cast<void**>(&bitmapHeader), nullptr, 0);
	ReleaseDC(nullptr, screenDC);

	if (!color)
	{
		Err() << "Failed to create cursor color bitmap" << std::endl;
		return false;
	}

	std::uint32_t* bitmapOffset = bitmapData;
	for (std::size_t remaining = _Size.X * _Size.Y; remaining > 0; --remaining, _Pixels += 4)
	{
		*bitmapOffset++ = static_cast<std::uint32_t>((_Pixels[3] << 24) | (_Pixels[0] << 16) | (_Pixels[1] << 8) | _Pixels[2]);
	}

	HBITMAP mask = CreateBitmap(static_cast<int>(_Size.X), static_cast<int>(_Size.Y), 1, 1, nullptr);

	if (!mask)
	{
		DeleteObject(color);
		Err() << "Failed to create cursor mask bitmap" << std::endl;
		return false;
	}

	auto cursorInfo = ICONINFO();
	cursorInfo.fIcon = FALSE;
	cursorInfo.xHotspot = _Hotspot.X;
	cursorInfo.yHotspot = _Hotspot.Y;
	cursorInfo.hbmColor = color;
	cursorInfo.hbmMask = mask;

	m_Cursor = reinterpret_cast<HCURSOR>(CreateIconIndirect(&cursorInfo));
	m_SystemCursor = false;

	DeleteObject(color);
	DeleteObject(mask);

	if (m_Cursor)
	{
		return true;
	}

	Err() << "Failed to create cursor from bitmaps" << std::endl;
	return false;
}

bool CursorData::loadFromSystem(CursorType _CursorType)
{
	Release();

	LPCTSTR shape = nullptr;

	switch (_CursorType)
	{
	case CursorData::CursorType::Arrow:
		shape = IDC_ARROW;
		break;
	
	case CursorData::CursorType::ArrowWait:
		shape = IDC_APPSTARTING; 
		break;
	
	case CursorData::CursorType::Wait:
		shape = IDC_WAIT;
		break;

	case CursorData::CursorType::Text:
		shape = IDC_IBEAM;
		break;

	case CursorData::CursorType::Hand:
		shape = IDC_HAND;
		break;

	case CursorData::CursorType::SizeHorizontal:
		shape = IDC_SIZEWE;
		break;

	case CursorData::CursorType::SizeVertical:
		shape = IDC_SIZENS;
		break;

	case CursorData::CursorType::SizeTopLeftBottomRight: 
		shape = IDC_SIZENWSE;
		break;

	case CursorData::CursorType::SizeBottomLeftTopRight:
		shape = IDC_SIZENESW;
		break;

	case CursorData::CursorType::SizeLeft:
		shape = IDC_SIZEWE;
		break;

	case CursorData::CursorType::SizeRight:
		shape = IDC_SIZEWE;
		break;

	case CursorData::CursorType::SizeTop:
		shape = IDC_SIZENS;
		break;

	case CursorData::CursorType::SizeBottom:
		shape = IDC_SIZENS;
		break;

	case CursorData::CursorType::SizeTopLeft:
		shape = IDC_SIZENWSE;
		break;

	case CursorData::CursorType::SizeBottomRight:
		shape = IDC_SIZENWSE;
		break;

	case CursorData::CursorType::SizeBottomLeft:
		shape = IDC_SIZENESW;
		break;

	case CursorData::CursorType::SizeTopRight:
		shape = IDC_SIZENESW;
		break;

	case CursorData::CursorType::SizeAll:
		shape = IDC_SIZEALL;
		break;

	case CursorData::CursorType::Cross:
		shape = IDC_CROSS;
		break;

	case CursorData::CursorType::Help:
		shape = IDC_HELP;
		break;

	case CursorData::CursorType::NotAllowed:
		shape = IDC_NO;
		break;
	}

	m_Cursor = LoadCursor(nullptr, shape);
	m_SystemCursor = true;

	if (m_Cursor)
	{
		return true;
	}

	Err() << "Could not create copy of a system cursor" << std::endl;
	return false;
}

void CursorData::Release()
{
	if (m_Cursor && !m_SystemCursor)
	{
		DestroyCursor(static_cast<HCURSOR>(m_Cursor));
		m_Cursor = nullptr;
	}
}

#endif