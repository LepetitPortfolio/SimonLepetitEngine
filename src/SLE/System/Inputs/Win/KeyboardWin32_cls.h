#pragma once
#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "../KeyboardPlatform.h"
#include "../../Common/String.h"



struct KeyboardWin32_cls : public KeyboardPlatform
{
   public:

	bool KeyPressed(Keyboard _Key);
	bool KeyPressed(ScanCode _ScaneCode);

	Keyboard GetKey(ScanCode _ScaneCode);
	ScanCode GetScanCode(Keyboard _Key);

	bool IsValidKey(Keyboard _Key);
	bool IsValidScanCode(ScanCode _ScanCode);

	String GetDescription(ScanCode _ScanCode);

	void SetVirtualKeyboardVisible(bool _Visible);

private:

	EnumArray<Keyboard, ScanCode, KeyCount> m_KeyToScanCodeMapping;
	EnumArray<ScanCode, Keyboard, ScanCodeCount> m_ScanCodeToKeyMapping;

	void EnsureMapping();

	Keyboard VirtualKeyToKey(UINT _VirtualKey);
	int KeyToVirtualKey(Keyboard _Key);
	WORD ScanCodeToWinScan(ScanCode _ScanCode);
	WORD ScanCodeToWinScanExtended(ScanCode _ScanCode);
	UINT ScanCodeToVirtualKey(ScanCode _ScanCode);
	std::optional<std::string> ScanCodeToConsumerKeyName(ScanCode code);


};
#endif