#pragma once
#include "../Platform.h"
#include "InputsGlobal.h"

#include "../Utils/EnumArray.h"

enum class JoystickAxis : InputID
{
	Unknown = -1,
	X = 0,
	Y,
	Z,
	R,
	U,
	V,
	PovX,
	PovY
};

static constexpr unsigned int JoystickCount = 8;
static constexpr unsigned int JoystickAxisCount = static_cast<unsigned int>(JoystickAxis::PovY) + 1;
static constexpr unsigned int JoystickButtonCount = 32;

struct JoystickIdentification
{
	std::string Name;
	unsigned int VendorID;
	unsigned int ProductID;
};

struct JoystickData
{
	unsigned int ButtonCount{};
	EnumArray<JoystickAxis, bool, JoystickAxisCount> Axes{};
};

struct JoystickState
{
	bool Connected{};
	EnumArray<JoystickAxis, bool, JoystickAxisCount> Axes{};
	std::array<bool, JoystickButtonCount> Buttons{};
};


class Joystick
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

	bool HasAxis(unsigned int _JoystickIndex, JoystickAxis _Axis) const;

	bool IsButtonPressed(unsigned int _JoystickIndex, unsigned int _Button) const;

	float GetAxisPosition(unsigned int _JoystickIndex, JoystickAxis _Axis) const;

	JoystickIdentification GetIdentification(unsigned int _JoystickIndex) const;

	JoystickState UpdateState();
	void Update();


#if PLATFORM_WINDOWS

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

#endif

private :

	JoystickIdentification m_Identification;
	JoystickState m_State;

#if PLATFORM_WINDOWS

	unsigned int m_JoystickIndex{};
	JOYCAPS m_JoystickData{};
	IDirectInputDevice8W* m_Device{};
	DIDEVCAPS m_DeviceData{};
	EnumArray<JoystickAxis, int, JoystickAxisCount> m_Axes{};
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

#elif PLATFORM_LINUX

	int m_File{ -1 };
	std::array<char, ABS_CNT> m_Mapping{};

#endif
};