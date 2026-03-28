#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "ClipboardWin32_cls.h"

#include "../../../Common/Error.h"
#include "../../../Common/Utils.h"

String ClipboardWin32_cls::GetString()
{
	String text;

	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		Err() << "Failed to get the clipboard data in Unicode format: " << GetErrorString(GetLastError()) << std::endl;
		return text;
	}

	if (!OpenClipboard(nullptr))
	{
		Err() << "Failed to open the Win32 clipboard: " << GetErrorString(GetLastError()) << std::endl;
		return text;
	}

	HANDLE clipboardHandle = GetClipboardData(CF_UNICODETEXT);

	if (!clipboardHandle)
	{
		Err() << "Failed to get Win32 handle for clipboard content: " << GetErrorString(GetLastError()) << std::endl;
		CloseClipboard();
		return text;
	}

	text = String(static_cast<wchar_t*>(GlobalLock(clipboardHandle)));
	GlobalUnlock(clipboardHandle);

	CloseClipboard();
	return text;
}

void ClipboardWin32_cls::SetString(const String& _Text)
{
	if (!OpenClipboard(nullptr))
	{
		Err() << "Failed to open the Win32 clipboard: " << GetErrorString(GetLastError()) << std::endl;
		return;
	}

	if (!EmptyClipboard())
	{
		Err() << "Failed to empty the Win32 clipboard: " << GetErrorString(GetLastError()) << std::endl;
		CloseClipboard();
		return;
	}


	const std::size_t stringSize = (_Text.GetSize() + 1) * sizeof(WCHAR);
	if (const HANDLE stringHandle = GlobalAlloc(GMEM_MOVEABLE, stringSize))
	{
		std::memcpy(GlobalLock(stringHandle), _Text.ToWideString().data(), stringSize);
		GlobalUnlock(stringHandle);
		SetClipboardData(CF_UNICODETEXT, stringHandle);
	}

	CloseClipboard();
}

#endif
