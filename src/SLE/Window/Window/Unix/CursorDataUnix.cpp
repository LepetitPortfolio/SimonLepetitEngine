
#include "../../../Platform.h"

#if PLATFORM_LINUX

#include "CursorDataUnix.h"

#include"Display.h" 
#include "../../../Common/Utils.h"

#include <vector>

#include <cassert>
#include <cstdlib>

template <>
struct XDeleter<XcursorImage>
{
	void operator()(XcursorImage* _CursorImage) const
	{
		XcursorImageDestroy(_CursorImage);
	}
};

CursorData::CursorData() : m_Display(OpenDisplay())
{
}

bool CursorData::LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
{
	Release();

	if (IsColorCursorSupported())
	{
		return LoadFromPixelsARGB(_Pixels, _Size, _Hotspot);
	}

	return LoadFromPixelsMonochrome(_Pixels, _Size, _Hotspot);
}

bool CursorData::loadFromSystem(CursorType _CursorType)
{
	Release();

	unsigned int shape = 0;

	switch (_CursorType)
	{
	default: return false;

	case CursorType::Arrow:
		shape = XC_arrow;
		break;
	case CursorType::Wait:
		shape = XC_watch;
		break;
	case CursorType::Text:
		shape = XC_xterm;
		break;
	case CursorType::Hand:
		shape = XC_hand2;
		break;
	case CursorType::SizeHorizontal:
		shape = XC_sb_h_double_arrow;
		break;
	case CursorType::SizeVertical:
		shape = XC_sb_v_double_arrow;
		break;
	case CursorType::SizeLeft:
		shape = XC_left_side;
		break;
	case CursorType::SizeRight:
		shape = XC_right_side;
		break;
	case CursorType::SizeTop:
		shape = XC_top_side;
		break;
	case CursorType::SizeBottom:
		shape = XC_bottom_side;
		break;
	case CursorType::SizeTopLeft:
		shape = XC_top_left_corner;
		break;
	case CursorType::SizeBottomRight:
		shape = XC_bottom_right_corner;
		break;
	case CursorType::SizeBottomLeft:
		shape = XC_bottom_left_corner;
		break;
	case CursorType::SizeTopRight:
		shape = XC_top_right_corner;
		break;
	case CursorType::SizeAll:
		shape = XC_fleur;
		break;
	case CursorType::Cross:
		shape = XC_crosshair;
		break;
	case CursorType::Help:
		shape = XC_question_arrow;
		break;
	case CursorType::NotAllowed:
		shape = XC_X_cursor;
		break;
	}

	m_Cursor = XCreateFontCursor(m_Display.get(), shape);

	return true;
}

bool CursorData::IsColorCursorSupported()
{
	return XcursorSupportsARGB(m_Display.get());
}

bool CursorData::LoadFromPixelsARGB(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
{
	const auto cursorImage = X11Ptr<XcursorImage>(XcursorImageCreate(static_cast<int>(_Size.X), static_cast<int>(_Size.Y)));
	cursorImage->xhot = _Hotspot.X;
	cursorImage->yhot = _Hotspot.Y;

	const std::size_t numPixel = std::size_t{ _Size.X } *std::size_t{ _Size.Y };

	for (size_t pixelIndex = 0; pixelIndex < numPixel; pixelIndex++)
	{
		cursorImage->pixels[pixelIndex] = static_cast<std::uint32_t>(_Pixels[pixelIndex * 4 + 2] + (_Pixels[pixelIndex * 4 + 1] << 8) +
			(_Pixels[pixelIndex * 4 + 0] << 16) + (_Pixels[pixelIndex * 4 + 3] << 24));
	}

	m_Cursor = XcursorImageLoadCursor(m_Display.get(), cursorImage.get());

	return true;
}

bool CursorData::LoadFromPixelsMonochrome(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
{
	const std::size_t packedWidth = (_Size.X + 7) / 8;
	const std::size_t bytes = packedWidth * _Size.Y;

	std::vector<std::uint8_t> mask(bytes, 0);
	std::vector<std::uint8_t> data(bytes, 0);

	for (std::size_t sizeYIndex = 0; sizeYIndex < _Size.Y; sizeYIndex++)
	{
		for (std::size_t sizeXIndex = 0; sizeXIndex < _Size.X; sizeXIndex++)
		{
			const std::size_t pixelIndex = sizeYIndex + sizeXIndex * _Size.X;
			const std::size_t byteIndex = sizeYIndex / 8 + sizeXIndex * packedWidth;
			const std::size_t bitIndex = sizeYIndex % 8;

			const std::uint8_t opacity = _Pixels[pixelIndex * 4 + 3] > 0;
			mask[byteIndex] |= static_cast<std::uint8_t>(opacity << bitIndex);

			const int intensity = (_Pixels[pixelIndex * 4 + 0] + _Pixels[pixelIndex * 4 + 1] + _Pixels[pixelIndex * 4 + 2]) / 3;
			const std::uint8_t bit = intensity > 128;
			data[byteIndex] |= static_cast<std::uint8_t>(bit << bitIndex);
		}
	}

	const Pixmap maskPixmap = XCreateBitmapFromData(m_Display.get(), XDefaultrootWindow(m_Display.get()), reinterpret_cast<char*>(mask.data()), _Size.X, _Size.Y);
	const Pixmap dataPixmap = XCreateBitmapFromData(m_Display.get(), XDefaultrootWindow(m_Display.get()), reinterpret_cast<char*>(data.data()), _Size.X, _Size.Y);

	XColor foregroundColor;
	XColor backgroundColor;
	foregroundColor.red = 0xFFFF;
	foregroundColor.blue = 0xFFFF;
	foregroundColor.green = 0xFFFF;
	backgroundColor.red = 0x0000;
	backgroundColor.blue = 0x0000;
	backgroundColor.green = 0x0000;

	m_Cursor = XCreatePixmapCursor(m_Display.get(), dataPixmap, maskPixmap, &foregroundColor, &backgroundColor, _Hotspot.X, _Hotspot.Y);

	XFreePixmap(m_Display.get(), dataPixmap);
	XFreePixmap(m_Display.get(), maskPixmap);

	return true;
}

void CursorData::Release()
{
	if (m_Cursor != None)
	{
		XFreeCursor(m_Display.get(), m_Cursor);
		m_Cursor = None;
	}
}

#endif