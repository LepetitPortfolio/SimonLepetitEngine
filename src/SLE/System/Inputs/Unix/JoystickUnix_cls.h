#pragma once
#include "../../PlatformConfig.h"

#if PLATFORM_LINUX


#include "../../../Common/EnumArray.h"


class JoystickUnix_cls
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

private :

	JoystickIdentification_e m_Identification;
	JoystickState m_State;


	int m_File{ -1 };
	std::array<char, ABS_CNT> m_Mapping{};

};

#endif
