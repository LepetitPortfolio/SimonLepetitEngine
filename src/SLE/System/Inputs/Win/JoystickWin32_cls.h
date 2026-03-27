#pragma once
#include "../../../PlatformConfig.h"

#if PLATFORM_WINDOWS

#include "InputsGlobal.h"

#include "../../Common/EnumArray.h"



class JoystickWin32_cls
{
public:

	static void Initialize();

	static void Cleanup();

	static bool IsConnected(unsigned int _JoystickIndex);
	bool IsConnected(unsigned int _JoystickIndex);

	bool Open(unsigned int _JoystickIndex);

	void Close();

	JoystickData GetJoystickData() const;

	unsigned int GetButtonCount(unsigned int _JoystickIndex) const;

	bool HasAxis(unsigned int _JoystickIndex, JoystickAxis_e _Axis) const;

	bool IsButtonPressed(unsigned int _JoystickIndex, unsigned int _Button) const;

	float GetAxisPosition(unsigned int _JoystickIndex, JoystickAxis_e _Axis) const;

	JoystickIdentification_e GetIdentification() const;

	JoystickState Update();

	static void SetLazyUpdate(bool _Status);

	static void UpdateConnections();

	static void InitializeInput();

	static void CleanupInput();

	static bool IsConnectedInput(unsigned int _JoystickIndex);

	static void UpdateConnectionsInput();
	
	bool OpenInput(unsigned int _JoystickIndex);

	void CloseInputs();

	JoystickData GetJoystickDataInput() const;

	JoystickState UpdateInputBuffered();

	JoystickState UpdateInputPolled();


private :

	JoystickIdentification_e m_Identification;
	JoystickState m_State;


	unsigned int m_JoystickIndex{};
	JOYCAPS m_JoystickData{};
	IDirectInputDevice8W* m_Device{};
	DIDEVCAPS m_DeviceData{};
	EnumArray<JoystickAxis_e, int, JoystickAxisCount> m_Axes{};
	std::array<int, JoystickButtonCount> m_Buttons{};
	
	bool m_Buffered{};
	
	static BOOL CALLBACK DeviceEnumerationCallback(const DIDEVICEINSTANCE* _DeviceInstance, void* _UserData);

	static BOOL CALLBACK DeviceObjectEnumerationCallback(const DIDEVICEOBJECTINSTANCE* _DeviceObjectInstance, void* _UserData);

	bool CreateDevice(const struct JoystickRecord& _Record);

	bool CheckVendorAndProductIDOfDevice();

	void GetFriendlyProductNameOfDevice();

	bool SetDeviceDataFormat();

	bool CheckDeviceCapabilities();

	bool EnumerateDeviceObjects();

	bool SetDeviceAxisModeToAbsolute();

	bool EnableBufferingBySettingBufferSize();

};
#endif