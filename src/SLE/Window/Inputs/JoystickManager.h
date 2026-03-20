#pragma once
#include "Joystick.h"

#include <array>

class JoystickManager
{
public:
	JoystickManager(const JoystickManager& _JoystickManager) = delete;

	JoystickManager& operator=(const JoystickManager& _JoystickManager) = delete;

	static JoystickManager& GetInstance();

	const JoystickData& GetJoystickData(unsigned int _JoystickIndex) const;

	const JoystickState& GetState(unsigned int _JoystickIndex) const;

	const JoystickIdentification& GetIdentification(unsigned int _JoystickIndex) const;

	void Update();

private:

	struct Item
	{
		Joystick Joystick;
		JoystickState State;
		JoystickData Data;
		JoystickIdentification Identification;
	};

	std::array<Item, JoystickCount> m_Joysticks;

	JoystickManager();

	~JoystickManager();

};