#pragma once
#include "../Core/Delegate/DelegateInclude.h"

#include "InputCommon.h"
#include "Gamepad.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <map>
#include <string>
#include <vector>

using ButtonActionCallback = Delegate<>;
using AxisActionCallback = Delegate<glm::vec2>;

struct InputProperties
{
	InputBase MainInput = -1;

	InputBase SeconderyInput = -1;

	ControllerType Controller = ControllerType::Unknown;

	InputStatus InputStatusWaitting = InputStatus::UnknownInput;

};

struct InputsAction
{
public:

	std::vector<InputProperties> InputPropertiesList;
	
	InputActionType InputAction = InputActionType::UnknownInput;

	ButtonActionCallback ButtonAction;
	AxisActionCallback AxisAction;
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Init();
	void Update();
	void Cleanup();

	void AddInputEvent(std::string _InputActionName, InputBase _NewInput, ButtonActionCallback _ButtonActionCallback);

	void ChangeInputEvent(std::string _InputActionName, InputBase _NewInputs, uint32_t _InputIndex = 0);

	void AddAxisEvent(std::string _InputActionName, InputBase NewInput, AxisActionCallback _AxisActionCallback);

	void ChangeAxisEvent(std::string _InputActionName, InputBase NewInput, uint32_t _InputIndex = 0);

private:

	std::map<std::string, InputsAction> m_InputActions;

	bool CheckActionValidation(ControllerType _Controller, InputBase _Input, InputStatus _InputStatusWaitting, InputBase _AdditionalInput = -1);

	void ExecuteAction(InputsAction& _InputAction, InputProperties& _InputPropertie);

};