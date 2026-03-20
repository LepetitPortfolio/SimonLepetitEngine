#include "Joystick.h"

#include "../Utils/Clock.h"
#include "../Utils/Time.h"
#include "../Utils/Error.h"
#include "../Utils/Utils.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <ostream>
#include <regstr.h>
#include <sstream>
#include <string>
#include <tchar.h>
#include <vector>

#include <cmath>

#if PLATFORM_WINDOWS

#ifndef DIDFT_OPTIONAL

#define DIDFT_OPTIONAL 0x80000000

#endif

GUID IID_IDirectInput8W = { 0xbf798031, 0x483a, 0x4da2, {0xaa, 0x99, 0x5d, 0x64, 0xed, 0x36, 0x97, 0x00} };

GUID GUID_XAxis = { 0xa36d02e0, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };
GUID GUID_YAxis = { 0xa36d02e1, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };
GUID GUID_ZAxis = { 0xa36d02e2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };
GUID GUID_RzAxis = { 0xa36d02e3, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };
GUID GUID_Slider = { 0xa36d02e4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };

GUID GUID_POV = { 0xa36d02f2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };

GUID GUID_RxAxis = { 0xa36d02f4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };
GUID GUID_RyAxis = { 0xa36d02f5, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };

HMODULE DInput8dll = nullptr;
IDirectInput8W DirectInput = nullptr;

struct JoystickRecord
{
	GUID Guid{};
	unsigned int Index{};
	bool Plugged{};
};

struct JoystickBlacklistEntry
{
	unsigned int VendorID{};
	unsigned int ProductID{};
};

struct ConnectionCache
{
	bool Connected{};
	Clock Timer;
};

using JoystickList = std::vector<JoystickRecord>;
JoystickList JoystickRList;

using JoystickBlacklist = std::vector<JoystickBlacklistEntry>;
JoystickBlacklist JoystickBlackList;

const DWORD DirectInputEventBufferSize = 32;

std::array<ConnectionCache, JoystickCount> ConnectionsCache{};

bool LazyUpdates = false;

std::string GetDeviceName(unsigned int _DeviceIndex, JOYCAPS _Data)
{
	static const std::string joystickDescription = "Unknown Joystick";

	LONG result = 0;
	HKEY rootKey = nullptr;
	HKEY currentKey = nullptr;
	std::basic_string<TCHAR> subKey;

	subKey = REGSTR_PATH_JOYCONFIG;
	subKey += TEXT('\\');
	subKey += _Data.szRegKey;
	subKey += TEXT('\\');
	subKey += REGSTR_KEY_JOYCURR;

	rootKey = HKEY_CURRENT_USER;
	result = RegOpenKeyEx(rootKey, subKey.c_str(), 0, KEY_READ, &currentKey);

	if (result != ERROR_SUCCESS)
	{
		rootKey = HKEY_LOCAL_MACHINE;
		result = RegOpenKeyEx(rootKey, subKey.c_str(), 0, KEY_READ, &currentKey);

		if (result != ERROR_SUCCESS)
		{
			Err() << "Unable to open registry for joystick at index " << _DeviceIndex << ": " << GetErrorString(static_cast<DWORD>(result)) << std::endl;
			return joystickDescription;
		}
	}

	std::basic_ostringstream<TCHAR> indexString;
	indexString << _DeviceIndex + 1;

	subKey = TEXT("Joystick");
	subKey += indexString.str();
	subKey += REGSTR_VAL_JOYOEMNAME;

	std::array<TCHAR, 256> keyData{};
	DWORD keyDataSize = sizeof(keyData);

	result = RegQueryValueEx(currentKey, subKey.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(keyData.data()), &keyDataSize);
	RegCloseKey(currentKey);

	if (result != ERROR_SUCCESS)
	{
		Err() << "Unable to query registry key for joystick at index " << _DeviceIndex << ": " << GetErrorString(static_cast<DWORD>(result)) << std::endl;
		return joystickDescription;
	}

	subKey = REGSTR_PATH_JOYOEM;
	subKey += TEXT('\\');
	subKey.append(keyData.data(), keyDataSize / sizeof(TCHAR));

	result = RegOpenKeyEx(rootKey, subKey.c_str(), 0, KEY_READ, &currentKey);

	if (result != ERROR_SUCCESS)
	{
		Err() << "Unable to open registry key for joystick at index " << _DeviceIndex << ": " << GetErrorString(static_cast<DWORD>(result)) << std::endl;
		return joystickDescription;
	}

	keyDataSize = sizeof(keyData);

	result = RegQueryValueEx(currentKey, REGSTR_VAL_JOYNOEMNAME, nullptr, nullptr, reinterpret_cast<LPBYTE>(keyData.data()), &keyDataSize);
	RegCloseKey(currentKey);

	if (result != ERROR_SUCCESS)
	{
		Err() << "Unable to query name for joystick at index " << _DeviceIndex << ": " << GetErrorString(static_cast<DWORD>(result)) << std::endl;
		return joystickDescription;
	}

	keyData.back() = TEXT('\0');
	return keyData;
}


void Joystick::Initialize()
{
	InitializeInput();

	if (!DirectInput)
	{
		Err() << "DirectInput value not available, falling back  to Windows joystick API" << std::endl;
	}

	UpdateConnections();
}

void Joystick::Cleanup()
{
	CleanupInput();
}

bool Joystick::IsConnected(unsigned int _JoystickIndex)
{
	if (DirectInput)
	{
		return IsConnectedInput(_JoystickIndex);
	}

	ConnectionCache& cache = ConnectionsCache[_JoystickIndex];
	Time connectionRefreshDeley = Milliseconds(500);
	if (!LazyUpdates && cache.Timer.GetElapsedTime() > connectionRefreshDeley)
	{
		JOYINFOEX joyInfo;
		joyInfo.dwSize = sizeof(joyInfo);
		joyInfo.dwFlags = 0;
		cache.Connected = joyGetPosEx(JOYSTICKID1 + _JoystickIndex, &joyInfo) == JOYERR_NOERROR;

		cache.Timer.Restart();
	}

	return cache.Connected;
}

bool Joystick::Open(unsigned int _JoystickIndex)
{
	if (DirectInput)
	{
		return OpenInput(_JoystickIndex);
	}

	m_JoystickIndex = JOYSTICKID1 + _JoystickIndex;

	const bool sucess = joyGetDevCaps(m_JoystickIndex, &m_JoystickData, sizeof(m_JoystickData)) == JOYERR_NOERROR;

	if (sucess)
	{
		m_Identification.Name = GetDeviceName(m_JoystickIndex, m_JoystickData);
		m_Identification.ProductID = m_JoystickData.wPid;
		m_Identification.VendorID = m_JoystickData.wMid;
	}

	return sucess;
}

void Joystick::Close()
{
	if (DirectInput)
	{
		CloseInputs();
	}
}

JoystickData Joystick::GetJoystickData() const
{
	if (DirectInput)
	{
		return GetJoystickDataInput();
	}

	JoystickData data;

	data.ButtonCount = std::min(m_JoystickData.wNumButtons, JoystickButtonCount);

	data.Axes[JoystickAxis::X] = true;
	data.Axes[JoystickAxis::Y] = true;
	data.Axes[JoystickAxis::Z] = (m_Data.wCaps & JOYCAPS_HASZ) != 0;
	data.Axes[JoystickAxis::R] = (m_Data.wCaps & JOYCAPS_HASR) != 0;
	data.Axes[JoystickAxis::U] = (m_Data.wCaps & JOYCAPS_HASU) != 0;
	data.Axes[JoystickAxis::V] = (m_Data.wCaps & JOYCAPS_HASV) != 0;
	data.Axes[JoystickAxis::PovX] = (m_Data.wCaps & JOYCAPS_HASPOV) != 0;
	data.Axes[JoystickAxis::PovY] = (m_Data.wCaps & JOYCAPS_HASPOV) != 0;

	return data;
}

JoystickIdentification Joystick::GetIdentification(unsigned int _JoystickIndex) const
{
	return m_Identification
}

JoystickState Joystick::UpdateState()
{
	if (DirectInput)
	{
		if (m_Buffered)
		{
			return UpdateInputBuffered();
		}

		return UpdateInputPolled();
	}

	JoystickState state;

	JOYINFOEX pos;
	pos.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | JOY_RETURNBUTTONS;
	pos.dwFlags |= (m_JoystickData.wCaps & JOYCAPS_POVCTS) ? JOY_RETURNPOVCTS : JOY_RETURNPOV;
	pos.dwSize = sizeof(JOYINFOEX);

	if (joyGetPosEx(m_JoystickIndex, &pos) == JOYERR_NOERROR)
	{
		state.Connected = true;

		state.Axes[JoystickAxis::X] = (static_cast<float>(pos.dwXpos) - static_cast<float>(m_JoystickData.wXmax + m_JoystickData.wXmin) / 2.f) * 200.f / static_cast<float>(m_JoystickData.wXmax - m_JoystickData.wXmin);
		state.Axes[JoystickAxis::Y] = (static_cast<float>(pos.dwYpos) - static_cast<float>(m_JoystickData.wYmax + m_JoystickData.wYmin) / 2.f) * 200.f / static_cast<float>(m_JoystickData.wYmax - m_JoystickData.wYmin);
		state.Axes[JoystickAxis::Z] = (static_cast<float>(pos.dwZpos) - static_cast<float>(m_JoystickData.wZmax + m_JoystickData.wZmin) / 2.f) * 200.f / static_cast<float>(m_JoystickData.wZmax - m_JoystickData.wZmin);
		state.Axes[JoystickAxis::R] = (static_cast<float>(pos.dwRpos) - static_cast<float>(m_JoystickData.wRmax + m_JoystickData.wRmin) / 2.f) * 200.f / static_cast<float>(m_JoystickData.wRmax - m_JoystickData.wRmin);
		state.Axes[JoystickAxis::U] = (static_cast<float>(pos.dwUpos) - static_cast<float>(m_JoystickData.wUmax + m_JoystickData.wUmin) / 2.f) * 200.f / static_cast<float>(m_JoystickData.wUmax - m_JoystickData.wUmin);
		state.Axes[JoystickAxis::V] = (static_cast<float>(pos.dwVpos) - static_cast<float>(m_JoystickData.wVmax + m_JoystickData.wVmin) / 2.f) * 200.f / static_cast<float>(m_JoystickData.wVmax - m_JoystickData.wVmin);

		if (pos.dwPOV != 0xFFFF)
		{
			const float angle = static_cast<float>(pos.dwPOV) / 18000.f * 3.141592654f;
			state.Axes[JoystickAxis::PovX] = std::sin(angle) * 100;
			state.Axes[JoystickAxis::PovY] = std::cos(angle) * 100;
		}
		else
		{
			state.Axes[JoystickAxis::PovX] = 0;
			state.Axes[JoystickAxis::PovY] = 0;
		}

		for (unsigned int index = 0; index < JoystickButtonCount; index++)
		{
			state.Buttons[index] = (pos.dwButtons & (1u << index)) != 0;
		}
	}

	return state;
}

void Joystick::SetLazyUpdate(bool _Status)
{
	LazyUpdates = _Status;
}

void Joystick::UpdateConnections()
{
	if (DirectInput)
	{
		UpdateConnectionsInput();
		return;
	}

	for (unsigned int index = 0; index < JoystickCount; index++)
	{
		JOYINOEX joyInfo;
		joyInfo.dwSize = sizeof(joyInfo);
		joyInfo.dwFlags = 0;

		ConnectionCache& cache = ConnectionsCache[index];
		cache.Connected = joyGetPosEx(JOYSTICKID1 + index, &joyInfo) == JOYERR_NOERROR;

		cache.Timer.Restart();
	}
}

void Joystick::InitializeInput()
{
	DInput8dll = LoadLibraryA("dinput8.dll");

	if (DInput8dll)
	{
		using DirectInput8CreateFunc = HRESULT(WINAPI*)(HINSTANCE, DWORD, const IID&, LPVOID*, LPUNKNOWN);
		auto directInput8Create = reinterpret_cast<DirectInput8CreateFunc>(reinterpret_cast<void*>(GetProcAddress(DInput8dll, "DirectInput8Create")));

		if (directInput8Create)
		{
			const HRESULT result = directInput8Create(GetModuleHandleW(nullptr), 0x0800, IID_IDirectInputDevice8W, reinterpret_cast<void**>(&DirectInput), nullptr);

			if (FAILED(result))
			{
				DirectInput = nullptr;
				FreeLibrary(DInput8dll);
				DInput8dll = nullptr;

				Err() << "Failed to initialize DirectInput: " << result << std::endl;
			}
		}
		else
		{
			FreeLibrary(DInput8dll);
			DInput8dll = nullptr;
		}
	}
}

void Joystick::CleanupInput()
{
	if (DirectInput)
	{
		DirectInput->Release();
		DirectInput = nullptr;
	}

	if (DInput8dll)
	{
		FreeLibrary(DInput8dll);
	}
}

bool Joystick::IsConnectedInput(unsigned int _JoystickIndex)
{
	return std::any_of(JoystickRList.cbegin(), JoystickRList.cend(), [_JoystickIndex](const JoystickRecord& record) { return record.Index == _JoystickIndex; });
}

void Joystick::UpdateConnectionsInput()
{
	for (JoystickRecord& record : JoystickRList)
	{
		record.Plugged = false;
	}

	const HRESULT result = DirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, &Joystick::DeviceEnumerationCallback, nullptr, DIEDBSFL_ATTACHEDONLY);

	JoystickRList.erase(std::remove_if(JoystickRList.begin(), JoystickRList.end(), [](const JoystickRecord& joystickRecord) {return !joystickRecord.Plugged; }), JoystickRList.end());

	if (FAILED(result))
	{
		Err() << "Failed to enumerate DirectInput devices: " << result << std::endl;
		return;
	}

	for (unsigned int index = 0; index < JoystickCount; index++)
	{
		unsigned int joystickRecordIndex = 0;
		bool loopDone = false;

		while ((!loopDone) && (joystickRecordIndex < JoystickRList.size()))
		{
			JoystickRecord& record = JoystickRList[joystickRecordIndex];
			if (record.Index == index)
			{
				loopDone = true;
			}

			if ((!loopDone) && (record.Index == JoystickCount))
			{
				record.Index = index;
				loopDone = true;
			}
			joystickRecordIndex++;
		}

	}
}

bool Joystick::OpenInput(unsigned int _JoystickIndex)
{
	m_Device = nullptr;

	m_Axes.fill(-1);
	m_Buttons.fill(-1);

	m_DeviceData = {};
	m_DeviceData.dwSize = sizeof(DIDEVCAPS);
	m_State = JoystickState();
	m_Buffered = false;

	unsigned int recordIndex = 0;
	bool breakLoop = false;
	bool outResult = false;

	while ((!breakLoop) && (recordIndex < JoystickRList.size()))
	{
		const JoystickRecord& record = JoystickRList[recordIndex];
		recordIndex++;

		if (record.Index == _JoystickIndex)
		{
			if (!CreateDevice(record))
			{
				breakLoop = true;
				outResult = false;
			}

			if (!breakLoop)
			{
				if (!GetVendorAndProductIDOfDevice())
				{
					breakLoop = true;
					outResult = false;
				}
			}

			if (!breakLoop)
			{
				GetFriendlyProductNameOfDevice();

				if (!SetDeviceDataFormat())
				{
					breakLoop = true;
					outResult = false;
				}
			}

			if (!breakLoop)
			{
				if (!CheckDeviceCapabilities())
				{
					breakLoop = true;
					outResult = false;
				}
			}

			if (!breakLoop)
			{
				if (!EnumerateDeviceObjects())
				{
					breakLoop = true;
					outResult = false;
				}
			}

			if (!breakLoop)
			{
				if (!SetDeviceAxisModeToAbsolute())
				{
					breakLoop = true;
					outResult = false;
				}
			}

			if (!breakLoop)
			{
				outResult = EnableBufferingBySettingBufferSize();
				breakLoop = true;
			}
		}
	}

	return outResult;
}

bool Joystick::CreateDevice(const JoystickRecord& _Record)
{
	HRESULT result = DirectInput->CreateDevice(_Record.Guid, &m_Device, nullptr);

	if (FAILED(result))
	{
		Err() << "Failed to create DirectInput device: " << result << std::endl;

		return false;
	}

	return true;
}

bool Joystick::CheckVendorAndProductIDOfDevice()
{
	auto property = DIPROPDWORD();
	property.diph.dwSize = sizeof(property);
	property.diph.dwHeaderSize = sizeof(property.diph);
	property.diph.dwHow = DIPH_DEVICE;

	if (SUCESSED(m_Device->GetProperty(DIPROP_VIDPID, &property.diph)))
	{
		bool outResult = true;

		m_Identification.ProductID = HIWORD(property.dwData);
		m_Identification.VendorID = LOWORD(property.dwData);

		if (m_Identification.ProductID && m_Identification.VendorID)
		{
			unsigned int blacklistEntryIndex = 0;
			bool breakLoop = false;

			while ((!breakLoop) && (blacklistEntryIndex < JoystickBlackList.size()))
			{
				const JoystickBlacklistEntry& blacklistEntry = JoystickBlackList[blacklistEntryIndex];
				blacklistEntryIndex++;

				if ((m_Identification.ProductID == blacklistEntry.ProductID) && (m_Identification.VendorID == blacklistEntry.VendorID))
				{
					m_Device->Release();
					m_Device = nullptr;

					breakLoop = true;
					outResult = false;
				}
			}
		}

		return outResult;
	}
}

void Joystick::GetFriendlyProductNameOfDevice()
{
	auto stringProperty = DIPROPSTRING();
	stringProperty.diph.dwSize = sizeof(stringProperty);
	stringProperty.diph.dwHeaderSize = sizeof(stringProperty.diph);
	stringProperty.diph.dwHow = DIPH_DEVICE;
	stringProperty.diph.dwObj = 0;

	if (SUCCEEDED(m_Device->GetProperty(DIPROP_PRODUCTNAME, &stringProperty.diph)))
	{
		m_Identification.Name = stringProperty.wsz;
	}
}

bool Joystick::SetDeviceDataFormat()
{
	static bool formatInitialized = false;
	static DIDATAFORMAT format;

	if (!formatInitialized)
	{
		const DWORD axisType = DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE;
		const DWORD povType = DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE;
		const DWORD buttonType = DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE;

		static std::array<DIOBJECTDATAFORMAT, 8 * 4 + 4 + JoystickButtonCount> data{};

		for (std::size_t index = 0; index < 4; index++)
		{
			data[8 * index + 0].pguid = GUID_XAxis;
			data[8 * index + 1].pguid = GUID_YAxis;
			data[8 * index + 2].pguid = GUID_ZAxis;
			data[8 * index + 3].pguid = GUID_RxAxis;
			data[8 * index + 4].pguid = GUID_RyAxis;
			data[8 * index + 5].pguid = GUID_RzAxis;
			data[8 * index + 6].pguid = GUID_Slider;
			data[8 * index + 7].pguid = GUID_Slider;
		}

		data[0].dwOfs = DIJOFS_X;
		data[1].dwOfs = DIJOFS_Y;
		data[2].dwOfs = DIJOFS_Z;
		data[3].dwOfs = DIJOFS_RX;
		data[4].dwOfs = DIJOFS_RY;
		data[5].dwOfs = DIJOFS_RZ;
		data[6].dwOfs = DIJOFS_SLIDER(0);
		data[7].dwOfs = DIJOFS_SLIDER(1);
		data[8].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lVX);
		data[9].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lVY);
		data[10].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lVZ);
		data[11].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lVRx);
		data[12].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lVRy);
		data[13].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lVRz);
		data[14].dwOfs = FIELD_OFFSET(DIJOYSTATE2, rglVSlider[0]);
		data[15].dwOfs = FIELD_OFFSET(DIJOYSTATE2, rglVSlider[1]);
		data[16].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lAX);
		data[17].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lAY);
		data[18].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lAZ);
		data[19].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lARx);
		data[20].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lARy);
		data[21].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lARz);
		data[22].dwOfs = FIELD_OFFSET(DIJOYSTATE2, rglASlider[0]);
		data[23].dwOfs = FIELD_OFFSET(DIJOYSTATE2, rglASlider[1]);
		data[24].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lFX);
		data[25].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lFY);
		data[26].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lFZ);
		data[27].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lFRx);
		data[28].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lFRy);
		data[29].dwOfs = FIELD_OFFSET(DIJOYSTATE2, lFRz);
		data[30].dwOfs = FIELD_OFFSET(DIJOYSTATE2, rglFSlider[0]);
		data[31].dwOfs = FIELD_OFFSET(DIJOYSTATE2, rglFSlider[1]);

		for (std::size_t index = 0; index < 8 * 4; index++)
		{
			data[index].dwType = axisType;
			data[index].dwFlags = 0;
		}

		for (std::size_t index = 0; index < 4; index++)
		{
			data[8 * 4 + index].pguid = &GUID_POV;
			data[8 * 4 + index].dwOfs = static_cast<DWORD>(DIJOFS_POV(static_cast<unsigned int>(index)));
			data[8 * 4 + index].dwType = povType;
			data[8 * 4 + index].dwFlags = 0;
		}

		for (unsigned int index = 0; index < JoystickButtonCount; index++)
		{
			data[8 * 4 + 4 + index].pguid = nullptr;
			data[8 * 4 + 4 + index].dwOfs = static_cast<DWORD>(DIJOFS_BUTTON(index));
			data[8 * 4 + 4 + index].dwType = buttonType;
			data[8 * 4 + 4 + index].dwFlags = 0;
		}

		format.dwSize = size(DIDATAFORMAT);
		format.dwObjSize = sizeof(DIOBJECTDATAFORMAT);
		format.dwFlags = DIDFT_ABSAXIS;
		format.dwDataSize = sizeof(DIJOYSTATE2);
		format.dwNumObjs = 8 * 4 + 4 + JoystickButtonCount;
		format.rgodf = data.data();

		formatInitialized = true;
	}

	HRESULT result = m_Device->SetDataFormat(&format);

	if (FAILED(result))
	{
		Err() << "Failed to set DirectInput device data format: " << result << std::endl;

		m_device->Release();
		m_device = nullptr;

		return false;
	}

	return true;

}

bool Joystick::CheckDeviceCapabilities()
{
	HRESULT result = m_Device->GetCapabilities(&m_DeviceData);

	if (FAILED(result))
	{
		Err() << "Failed to set DirectInput device data format: " << result << std::endl;

		m_device->Release();
		m_device = nullptr;

		return false;
	}

	return true;
}

bool Joystick::EnumerateDeviceObjects()
{
	HRESULT result = m_Device->EnumObjects(&JoystickImpl::deviceObjectEnumerationCallback, this, DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV);

	if (FAILED(result))
	{
		err() << "Failed to enumerate DirectInput device objects: " << result << std::endl;

		m_device->Release();
		m_device = nullptr;

		return false;
	}

	return true;
}

bool Joystick::SetDeviceAxisModeToAbsolute()
{
	unsigned int axisIndex = 0;
	bool breakLoop = false;
	bool outResult = true;

	while ((!breakLoop) && (axisIndex < m_Axes.size()))
	{
		const int axis = m_Axes[axisIndex];
		axisIndex++;

		if (axis != -1)
		{
			auto property = DIPROPDWORD();;
			property.diph.dwSize = sizeof(property);
			property.diph.dwHeaderSize = sizeof(property.diph);
			property.diph.dwHow = DIPH_DEVICE;
			property.diph.dwObj = 0;

			HRESULT result = m_Device->GetProperty(DIPROP_AXISMODE, &property.diph);

			if (FAILED(result))
			{
				Err() << "Failed to get DirectInput device axis mode for device " << std::quoted(m_Identification.Name) << ": " << result << std::endl;

				m_Device->Release();
				m_Device = nullptr;

				breakLoop = true;
				outResult = false;
			}

			if (property.dwData != DIPROPAXISMODE_ABS)
			{
				property = {};
				property.diph.dwSize = sizeof(property);
				property.diph.dwHeaderSize = sizeof(property.diph);
				property.diph.dwHow = DIPH_DEVICE;
				property.dwData = DIPROPAXISMODE_ABS;

				m_Device->SetProperty(DIPROP_AXISMODE, &property.diph);

				property = {};
				property.diph.dwSize = sizeof(property);
				property.diph.dwHeaderSize = sizeof(property.diph);
				property.diph.dwHow = DIPH_DEVICE;
				property.diph.dwObj = 0;

				result = m_Device->GetProperty(DIPROP_AXISMODE, &property.diph);

				if (FAILED(result))
				{
					err() << "Failed to verify DirectInput device axis mode for device "
						<< std::quoted(m_identification.name.toAnsiString()) << ": " << result << std::endl;

					m_Device->Release();
					m_Device = nullptr;

					breakLoop = true;
					outResult = false;
				}

				if ((!breakLoop) && (property.dwData != DIPROPAXISMODE_ABS))
				{
					if ((m_Identification.VendorID) && (m_Identification.ProductID))
					{
						JoystickBlacklistEntry entry{};
						entry.VendorID = m_Identification.VendorID;
						entry.ProductID = m_Identification.ProductID;

						JoystickBlackList.push_back(entry);
						JoystickBlackList.shrink_to_fit();
					}

					m_Device->Release();
					m_Device = nullptr;

					breakLoop = true;
					outResult = false;
				}

				outResult = false;
			}
			else
			{
				breakLoop = true;
			}
		}
	}

	return outResult;
}

bool Joystick::EnableBufferingBySettingBufferSize()
{
	auto property = DIPROPDWORD();;
	property.diph.dwSize = sizeof(property);
	property.diph.dwHeaderSize = sizeof(property.diph);
	property.diph.dwHow = DIPH_DEVICE;
	property.dwData = DirectInputEventBufferSize;

	HRESULT result = m_Device->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

	if (result == DI_OK)
	{
		m_Buffered = true;
	}
	else if (result == DI_POLLEDDEVICE)
	{
		m_Buffered = false;
	}
	else
	{
		Err() << "Failed to set DirectInput device buffer size for device " << std::quoted(m_Identification.Name) << ": " << result << std::endl;

		m_Device->Release();
		m_Device = nullptr;

		return false;
	}

	return true;
}

void Joystick::CloseInputs()
{
	if (m_Device)
	{
		m_Device->Release();
		m_Device = nullptr;
	}
}

JoystickData Joystick::GetJoystickDataInput() const
{
	JoystickData joystickData;

	joystickData.ButtonCount = 0;

	for (const int button : m_Buttons)
	{
		if (button = : = -1)
		{
			joystickData.ButtonCount++;
		}
	}

	for (unsigned int axisIndex = 0 axisIndex < JoystickAxisCount; axisIndex++)
	{
		const auto axis = static_cast<JoystickAxis>(axisIndex);
		joystickData.Axes[axis] = (m_Axes[axis] != -1);
	}

	return joystickData;
}

JoystickState Joystick::UpdateInputBuffered()
{
	m_State.Connected = false;

	if (!m_Device)
	{
		return m_State;
	}

	std::array<DIDEVICEOBJECTDATA, DirectInputEventBufferSize> events{};
	DWORD eventCount = DirectInputEventBufferSize;

	HRESULT result = m_Device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events.data(), &eventCount, 0);

	if ((result == DIERR_NOTACQUIRED) || (result == DIERR_INPUTLOST))
	{
		m_Device->Acquire();
		result = m_Device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events.data(), &eventCount, 0);
	}

	if ((result == DIERR_NOTACQUIRED) || (result == DIERR_INPUTLOST))
	{
		m_Device->Release();
		m_Device = nullptr;

		return m_State;
	}

	if (FAILED(result))
	{
		Err() << "Failed to get DirectInput device data: " << result << std::endl;

		return m_State;
	}

	for (DWORD eventIndex = 0; eventIndex < eventCount; eventIndex++)
	{
		bool eventHandled = false;

		bool breakLoop = false;
		unsigned int axisIndex = 0;

		while ((!breakLoop) && (axisIndex < JoystickAxisCount))
		{
			const auto axis = static_cast<JoystickAxis>(axisIndex);
			axisIndex++;

			if (m_Axes[axis] == static_cast<int>(events[eventIndex].dwOfs))
			{
				if ((axis == JoystickAxis::PovX) || (axis == JoystickAxis::PovY))
				{
					const unsigned short value = LOWORD(events[eventIndex].dwData);

					if (value != 0xFFFF)
					{
						const float angle = (static_cast<float>(value)) * 3.141592654f / DI_DEGREES / 180.f;

						m_State.Axes[JoystickAxis::PovX] = std::sin(angle) * 100.f;
						m_State.Axes[JoystickAxis::PovY] = std::cos(angle) * 100.f;

					}
					else
					{
						m_State.Axes[JoystickAxis::PovX] = 0.f;
						m_State.Axes[JoystickAxis::PovY] = 0.f;
					}
				}
				else
				{
					m_State.Axes[axis] = (static_cast<float>(static_cast<short>(events[eventIndex].dwData)) + 0.5f) * 100.f / 32767.5f;
				}

				eventHandled = true;
				breakLoop = true;
			}
		}

		if (!eventHandled)
		{
			for (unsigned int buttonIndex = 0; buttonIndex < JoystickButtonCount; buttonIndex++)
			{
				if (m_Buttons[buttonIndex] == static_cast<int>(events[eventIndex].dwOfs))
				{
					m_State.Buttons[buttonIndex] = (events[eventIndex].dwData != 0);
				}
			}
		}
	}

	m_State.Connected = false;

	return m_State;
}

JoystickState Joystick::UpdateInputPolled()
{
	JoystickState state;

	if (m_Device)
	{
		m_Device->Poll();

		DIJOYSTATE2 joystate;

		HRESULT result = m_Device->GetDeviceState(sizeof(joystate), &joystate);

		if ((result == DIERR_NOTACQUIRED) || (result == DIERR_INPUTLOST))
		{
			m_Device->Acquire();
			m_Device->Poll();

			result = m_Device->GetDeviceState(sizeof(joystate), &joystate);
		}

		if ((result == DIERR_NOTACQUIRED) || (result == DIERR_INPUTLOST))
		{
			m_Device->Release();
			m_Device = nullptr;

			return m_State;
		}

		if (FAILED(result))
		{
			Err() << "Failed to get DirectInput device state: " << result << std::endl;

			return m_State;
		}

		for (unsigned int axisIndex = 0; axisIndex < JoystickAxisCount; axisIndex++)
		{
			const auto axis = static_cast<JoystickAxis>(axisIndex);

			if (m_Axes[axis] != -1)
			{
				if ((axis == JoystickAxis::PovX) || (axis == JoystickAxis::PovY))
				{
					const unsigned short value = LOWORD( *reinterpret_cast<const DWORD*>(reinterpret_cast<const char*>(&joystate) + m_Axes[axis]));

					if (value != 0xFFFF)
					{
						const float angle = (static_cast<float>(value)) * 3.141592654f / DI_DEGREES / 180.f;

						m_State.Axes[JoystickAxis::PovX] = std::sin(angle) * 100.f;
						m_State.Axes[JoystickAxis::PovY] = std::cos(angle) * 100.f;
					}
					else
					{
						m_State.Axes[JoystickAxis::PovX] = 0.f;
						m_State.Axes[JoystickAxis::PovY] = 0.f;
					}
				}
				else
				{
					m_State.Axes[axis] = (static_cast<float>(*reinterpret_cast<const LONG*>(reinterpret_cast<const char*>(&joystate) + m_Axes[axis])) + 0.5f) * 100.f / 32767.5f;
				}
			}
			else
			{
				m_State.Axes[axis] = 0.f;
			}
		}

		for (unsigned int buttonIndex = 0; i < JoystickButtonCount; ++i)
		{
			if (m_Buttons[buttonIndex] != -1)
			{
				const BYTE value = *reinterpret_cast<const BYTE*>(reinterpret_cast<const char*>(&joystate) + m_Buttons[buttonIndex]);

				m_State.Buttons[buttonIndex] = ((value & 0x80) != 0);
			}
			else
			{
				m_State.Buttons[buttonIndex] = false;
			}
		}

		m_State.Connected = true;
	}

	return m_State;
}

BOOL CALLBACK Joystick::DeviceEnumerationCallback(const DIDEVICEINSTANCE* _DeviceInstance, void* _UserData)
{
	bool breakLoop = false;
	unsigned int recordIndex = 0;

	while((!breakLoop) && (recordIndex < JoystickRList.size()))
	{
		JoystickRecord& record = JoystickRList[recordIndex];
		recordIndex++;

		if (record.Guid == _DeviceInstance->guidInstance)
		{
			record.Plugged = true;
			breakLoop = true;
		}
	}

	if (breakLoop)
	{
		return DIENUM_CONTINUE;
	}

	const JoystickRecord record = { _DeviceInstance->guidInstance, JoystickCount, true };
	JoystickRList.push_back(record);

	return DIENUM_CONTINUE;
}

BOOL CALLBACK Joystick::DeviceObjectEnumerationCallback(const DIDEVICEOBJECTINSTANCE* _DeviceObjectInstance, void* _UserData)
{
	Joystick& joystick = *reinterpret_cast<Joystick*>(_UserData);

	if (DIDFT_GETTYPE(_DeviceObjectInstance->dwType) & DIDFT_AXIS)
	{
		switch (_DeviceObjectInstance->guidType)
		{
		case GUID_XAxis:
			joystick.m_Axes[JoystickAxis::X] = DIJOFS_X;
			break;

		case GUID_YAxis:
			joystick.m_Axes[JoystickAxis::Y] = DIJOFS_Y;
			break;

		case GUID_ZAxis:
			joystick.m_Axes[JoystickAxis::Z] = DIJOFS_Z;
			break;

		case GUID_RzAxis:
			joystick.m_Axes[JoystickAxis::R] = DIJOFS_RZ;
			break;

		case GUID_RxAxis:
			joystick.m_Axes[JoystickAxis::U] = DIJOFS_RX;
			break;

		case GUID_RyAxis:
			joystick.m_Axes[JoystickAxis::V] = DIJOFS_RY;
			break;

		case GUID_Slider:
			if (joystick.m_Axes[JoystickAxis::U] == -1)
			{
				joystick.m_Axes[JoystickAxis::U] = DIJOFS_SLIDER(0);
			}
			else
			{
				joystick.m_Axes[JoystickAxis::U] = DIJOFS_SLIDER(1);
			}
			break;

		default:
			return DIENUM_CONTINUE;
		}

		auto propertyRanger = DIPROPRANGE();
		propertyRanger.diph.dwSize = sizeof(propertyRanger);
		propertyRanger.diph.dwHeaderSize = sizeof(propertyRanger.diph);
		propertyRanger.diph.dwObj = _DeviceObjectInstance.->dwType;
		propertyRanger.diph.dwHow = DIPH_BYID;
		propertyRanger.lMin = -32768;
		propertyRanger.lMax = 32767;

		const HRESULT result = joystick.m_Device->SetProperty(DIPROP_RANGE, &propertyRanger.diph);

		if (result != DI_OK)
		{
			Err() << "Failed to set DirectInput device axis property range: " << result << std::endl;
		}
	
		return DIENUM_CONTINUE;
	}

	if(DIDFT_GETTYPE(_DeviceObjectInstance->dwType) & DIDFT_POV)
	{
		if (_DeviceObjectInstance->guidType == GUID_POV)
		{
			if (joystick.m_Axes[JoystickAxis::PovX] == -1)
			{
				joystick.m_Axes[JoystickAxis::PovX] = DIJOFS_POV(0);
				joystick.m_Axes[JoystickAxis::PovY] = DIJOFS_POV(0);
			}
		}

		return DIENUM_CONTINUE;
	}

	if (DIDFT_GETTYPE(_DeviceObjectInstance->dwType) & DIDFT_BUTTON)
	{
		bool loopBreak = false;
		unsigned int buttonIndex = 0;

		while((!loopBreak) && (buttonIndex < JoystickButtonCount))
		{
			if (joystick.m_Buttons[buttonIndex] == -1)
			{
				joystick.m_Buttons[buttonIndex] = DIJOFS_BUTTON(static_cast<int>(buttonIndex));
				loopBreak = true;
			}
			else
			{
				buttonIndex++;
			}
		}

		return DIENUM_CONTINUE;
	}

	return DIENUM_CONTINUE;
}


#endif