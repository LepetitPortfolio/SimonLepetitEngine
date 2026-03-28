
#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS
#include "CursorWin32_cls.h"

#include "../../../Common/Error.h"

bool CursorWin32_cls::LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
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
		Err() << "Failed to create Cursor_cls color bitmap" << std::endl;
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
		Err() << "Failed to create Cursor_cls mask bitmap" << std::endl;
		return false;
	}

	auto Cursor_clsInfo = ICONINFO();
	Cursor_clsInfo.fIcon = FALSE;
	Cursor_clsInfo.xHotspot = _Hotspot.X;
	Cursor_clsInfo.yHotspot = _Hotspot.Y;
	Cursor_clsInfo.hbmColor = color;
	Cursor_clsInfo.hbmMask = mask;

	m_Cursor = reinterpret_cast<HCursor_cls>(CreateIconIndirect(&Cursor_clsInfo));
	m_SystemCursor_cls = false;

	DeleteObject(color);
	DeleteObject(mask);

	if (m_Cursor)
	{
		return true;
	}

	Err() << "Failed to create Cursor_cls from bitmaps" << std::endl;
	return false;
}

bool CursorWin32_cls::loadFromSystem(CursorType_e _CursorType)
{
	Release();

	LPCTSTR shape = nullptr;

	switch (_CursorType)
	{
	case CursorPlatform::CursorType_e::Arrow:
		shape = IDC_ARROW;
		break;
	
	case CursorPlatform::CursorType_e::ArrowWait:
		shape = IDC_APPSTARTING; 
		break;
	
	case CursorPlatform::CursorType_e::Wait:
		shape = IDC_WAIT;
		break;

	case CursorPlatform::CursorType_e::Text:
		shape = IDC_IBEAM;
		break;

	case CursorPlatform::CursorType_e::Hand:
		shape = IDC_HAND;
		break;

	case CursorPlatform::CursorType_e::SizeHorizontal:
		shape = IDC_SIZEWE;
		break;

	case CursorPlatform::CursorType_e::SizeVertical:
		shape = IDC_SIZENS;
		break;

	case CursorPlatform::CursorType_e::SizeTopLeftBottomRight: 
		shape = IDC_SIZENWSE;
		break;

	case CursorPlatform::CursorType_e::SizeBottomLeftTopRight:
		shape = IDC_SIZENESW;
		break;

	case CursorPlatform::CursorType_e::SizeLeft:
		shape = IDC_SIZEWE;
		break;

	case CursorPlatform::CursorType_e::SizeRight:
		shape = IDC_SIZEWE;
		break;

	case CursorPlatform::CursorType_e::SizeTop:
		shape = IDC_SIZENS;
		break;

	case CursorPlatform::CursorType_e::SizeBottom:
		shape = IDC_SIZENS;
		break;

	case CursorPlatform::CursorType_e::SizeTopLeft:
		shape = IDC_SIZENWSE;
		break;

	case CursorPlatform::CursorType_e::SizeBottomRight:
		shape = IDC_SIZENWSE;
		break;

	case CursorPlatform::CursorType_e::SizeBottomLeft:
		shape = IDC_SIZENESW;
		break;

	case CursorPlatform::CursorType_e::SizeTopRight:
		shape = IDC_SIZENESW;
		break;

	case CursorPlatform::CursorType_e::SizeAll:
		shape = IDC_SIZEALL;
		break;

	case CursorPlatform::CursorType_e::Cross:
		shape = IDC_CROSS;
		break;

	case CursorPlatform::CursorType_e::Help:
		shape = IDC_HELP;
		break;

	case CursorPlatform::CursorType_e::NotAllowed:
		shape = IDC_NO;
		break;
	}

	m_Cursor = LoadCursor_cls(nullptr, shape);
	m_SystemCursor_cls = true;

	if (m_Cursor)
	{
		return true;
	}

	Err() << "Could not create copy of a system Cursor_cls" << std::endl;
	return false;
}

void CursorWin32_cls::Release()
{
	if (m_Cursor && !m_SystemCursor_cls)
	{
		DestroyCursor_cls(static_cast<HCursor_cls>(m_Cursor));
		m_Cursor = nullptr;
	}
}

#endif