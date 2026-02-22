#pragma once
#include "../Platform.h"
#include "../Utils/EnumArray.h"

#include "Mouse.h"
#include "Keyboard.h"

#include <string>
#include <optional>

class Inputs
{
public:

	bool KeyPressed(Keyboard _Key);
	bool KeyPressed(ScanCode _ScaneCode);

	Keyboard GetKey(ScanCode _ScaneCode);
	ScanCode GetScanCode(Keyboard _Key);

	bool IsValidKey(Keyboard _Key);
	bool IsValidScanCode(ScanCode _ScanCode);

	std::string GetDescription(ScanCode _ScanCode);

	void SetVirtualKeyboardVisible(bool _Visible);

private:

	EnumArray<Keyboard, ScanCode, KeyCount> m_KeyToScanCodeMapping;
	EnumArray<ScanCode, Keyboard, ScanCodeCount> m_ScanCodeToKeyMapping;

	void EnsureMapping();

#if PLATFORM_WINDOWS
	Keyboard VirtualKeyToKey(UINT _VirtualKey);
	int KeyToVirtualKey(Keyboard _Key);
	WORD ScanCodeToWinScan(ScanCode _ScanCode);
	WORD ScanCodeToWinScanExtended(ScanCode _ScanCode);
	UINT ScanCodeToVirtualKey(ScanCode _ScanCode);
	std::optional<std::string> ScanCodeToConsumerKeyName(ScanCode code);
#elif PLATFORM_LINUX

#endif

};