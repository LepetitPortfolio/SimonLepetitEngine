#pragma once
#include "Joystick.h"
#include "JoystickPlatform.h"

#include <array>

class JoystickManager_cls
{
public:
	JoystickManager_cls(const JoystickManager_cls& _JoystickManager) = delete;

	JoystickManager_cls& operator=(const JoystickManager_cls& _JoystickManager) = delete;

	static JoystickManager_cls& GetInstance();

	const JoystickData& GetJoystickData(unsigned int _JoystickIndex) const;

	const JoystickState& GetState(unsigned int _JoystickIndex) const;

	const JoystickIdentification_e& GetIdentification(unsigned int _JoystickIndex) const;

	void Update();

private:

	struct Item
	{
		JoystickPlatform Joystick;
		JoystickState State;
		JoystickData Data;
		JoystickIdentification_e Identification;
	};

	std::array<Item, JoystickCount> m_Joysticks;

	JoystickManager_cls();

	~JoystickManager_cls();

};