#pragma once

#include "../PlatformConfig.h"

#include "String.h"

#if PLATFORM_WINDOWS

inline std::string GetErrorString(DWORD _Error)
{
	PTCHAR buffer = nullptr;

	DWORD flags = FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;

	if (FormatMessage(flags, nullptr, _Error, 0, reinterpret_cast<PTCHAR>(&buffer), 0, nullptr) == 0)
	{
		return "Unknown error !";
	}

	const String message = buffer;
	LocalFree(buffer);
	return message.ToAnsiString();
}

#elif PLATFORM_LINUX

#endif