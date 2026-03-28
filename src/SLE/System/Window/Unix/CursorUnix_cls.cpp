
#include "../../PlatformConfig.h"

#if PLATFORM_LINUX

#include "CursorUnix_cls.h"
#include "Display.h"
#include "../../../Common/Utils.h"

#include <vector>

#include <cassert>
#include <cstdlib>

template <>
struct XDeleter<XCursor_clsImage>
{
	void operator()(XCursor_clsImage* _Cursor_clsImage) const
	{
		XCursor_clsImageDestroy(_Cursor_clsImage);
	}
};

CursorUnix_cls::CursorUnix_cls() : m_Display(OpenDisplay())
{
}

CursorUnix_cls::~CursorUnix_cls()
{
	Release();
}

bool CursorUnix_cls::LoadFromPixels(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
{
	Release();

	if (IsColorCursor_clsSupported())
	{
		return LoadFromPixelsARGB(_Pixels, _Size, _Hotspot);
	}

	return LoadFromPixelsMonochrome(_Pixels, _Size, _Hotspot);
}

bool CursorUnix_cls::LoadFromSystem(Cursor_cls::CursorType_e _CursorType)
{
	Release();

	unsigned int shape = 0;

	switch (_CursorType)
	{
	default: return false;

	case Cursor_cls::CursorType_e::Arrow:
		shape = XC_arrow;
		break;
	case Cursor_cls::CursorType_e::Wait:
		shape = XC_watch;
		break;
	case Cursor_cls::CursorType_e::Text:
		shape = XC_xterm;
		break;
	case Cursor_cls::CursorType_e::Hand:
		shape = XC_hand2;
		break;
	case Cursor_cls::CursorType_e::SizeHorizontal:
		shape = XC_sb_h_double_arrow;
		break;
	case Cursor_cls::CursorType_e::SizeVertical:
		shape = XC_sb_v_double_arrow;
		break;
	case Cursor_cls::CursorType_e::SizeLeft:
		shape = XC_left_side;
		break;
	case Cursor_cls::CursorType_e::SizeRight:
		shape = XC_right_side;
		break;
	case Cursor_cls::CursorType_e::SizeTop:
		shape = XC_top_side;
		break;
	case Cursor_cls::CursorType_e::SizeBottom:
		shape = XC_bottom_side;
		break;
	case Cursor_cls::CursorType_e::SizeTopLeft:
		shape = XC_top_left_corner;
		break;
	case Cursor_cls::CursorType_e::SizeBottomRight:
		shape = XC_bottom_right_corner;
		break;
	case Cursor_cls::CursorType_e::SizeBottomLeft:
		shape = XC_bottom_left_corner;
		break;
	case Cursor_cls::CursorType_e::SizeTopRight:
		shape = XC_top_right_corner;
		break;
	case Cursor_cls::CursorType_e::SizeAll:
		shape = XC_fleur;
		break;
	case Cursor_cls::CursorType_e::Cross:
		shape = XC_crosshair;
		break;
	case Cursor_cls::CursorType_e::Help:
		shape = XC_question_arrow;
		break;
	case Cursor_cls::CursorType_e::NotAllowed:
		shape = XC_X_Cursor_cls;
		break;
	}

	m_Cursor = XCreateFontCursor_cls(m_Display.get(), shape);

	return true;
}

bool CursorUnix_cls::IsColorCursor_clsSupported()
{
	return XCursor_clsSupportsARGB(m_Display.get());
}

bool CursorUnix_cls::LoadFromPixelsARGB(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
{
	const auto Cursor_clsImage = X11Ptr<XCursor_clsImage>(XCursor_clsImageCreate(static_cast<int>(_Size.X), static_cast<int>(_Size.Y)));
	Cursor_clsImage->xhot = _Hotspot.X;
	Cursor_clsImage->yhot = _Hotspot.Y;

	const std::size_t numPixel = std::size_t{ _Size.X } *std::size_t{ _Size.Y };

	for (size_t pixelIndex = 0; pixelIndex < numPixel; pixelIndex++)
	{
		Cursor_clsImage->pixels[pixelIndex] = static_cast<std::uint32_t>(_Pixels[pixelIndex * 4 + 2] + (_Pixels[pixelIndex * 4 + 1] << 8) +
			(_Pixels[pixelIndex * 4 + 0] << 16) + (_Pixels[pixelIndex * 4 + 3] << 24));
	}

	m_Cursor = XCursor_clsImageLoadCursor_cls(m_Display.get(), Cursor_clsImage.get());

	return true;
}

bool CursorUnix_cls::LoadFromPixelsMonochrome(const std::uint8_t* _Pixels, Vector2u _Size, Vector2u _Hotspot)
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

	const Pixmap maskPixmap = XCreateBitmapFromData(m_Display.get(), XDefaultRootWindow(m_Display.get()), reinterpret_cast<char*>(mask.data()), _Size.X, _Size.Y);
	const Pixmap dataPixmap = XCreateBitmapFromData(m_Display.get(), XDefaultRootWindow(m_Display.get()), reinterpret_cast<char*>(data.data()), _Size.X, _Size.Y);

	XColor foregroundColor;
	XColor backgroundColor;
	foregroundColor.red = 0xFFFF;
	foregroundColor.blue = 0xFFFF;
	foregroundColor.green = 0xFFFF;
	backgroundColor.red = 0x0000;
	backgroundColor.blue = 0x0000;
	backgroundColor.green = 0x0000;

	m_Cursor = XCreatePixmapCursor_cls(m_Display.get(), dataPixmap, maskPixmap, &foregroundColor, &backgroundColor, _Hotspot.X, _Hotspot.Y);

	XFreePixmap(m_Display.get(), dataPixmap);
	XFreePixmap(m_Display.get(), maskPixmap);

	return true;
}

void CursorUnix_cls::Release()
{
	if (m_Cursor != WindowStyle_e::None)
	{
		XFreeCursor_cls(m_Display.get(), m_Cursor);
		m_Cursor = WindowStyle_e::None;
	}
}

#endif