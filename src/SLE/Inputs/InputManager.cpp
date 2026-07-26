#include "InputManager.h"

InputManager::InputManager()
{

}

InputManager::~InputManager()
{

}

void InputManager::Init()
{

}

void InputManager::Update()
{
	for (auto inputAction : m_InputActions)
	{
		std::vector<InputProperties>* inputProperties = &inputAction.second.InputPropertiesList;
		uint32_t inputPropertiesIndex = 0;
		bool actionTriggered = false;

		while ((!actionTriggered) && (inputPropertiesIndex < inputProperties->size()))
		{
			InputProperties* inputPropertie = &inputProperties->at(inputPropertiesIndex);

			if (CheckActionValidation(inputPropertie->Controller, inputPropertie->MainInput, inputPropertie->InputStatusWaitting, inputPropertie->SeconderyInput))
			{
				actionTriggered = true;
			}
			else
			{
				inputPropertiesIndex++;
			}
		}
		
	}
}

void InputManager::Cleanup()
{

}


bool InputManager::CheckActionValidation(ControllerType _Controller, InputBase _Input, InputStatus _InputStatusWaitting, InputBase _AdditionalInput)
{
	InputStatus detectedInputStatus = InputStatus::UnknownInput;
	
	switch (_Controller)
	{
	case ControllerType::Unknown:
		break;
	case ControllerType::Keyboard:
		detectedInputStatus = Keyboard::GetKeyInputStatut((KeyboardInput)_Input);
		break;
	case ControllerType::Mouse:
		detectedInputStatus = Mouse::GetMouseButtonStatu((MouseButton)_Input);
		break;
	case ControllerType::Gamepad:
		//detectedInputStatus = Gamepad::GetMouseButtonStatu((MouseButton)_Input);
		break;
	default:
		break;
	}

	bool additionalInputValidation = true;

	if (_AdditionalInput != -1)
	{
		additionalInputValidation = CheckActionValidation(_Controller, _AdditionalInput, _InputStatusWaitting);
	}

	return additionalInputValidation && (detectedInputStatus == _InputStatusWaitting);
}

void InputManager::ExecuteAction(InputsAction& _InputAction, InputProperties& _InputPropertie)
{
	switch (_InputAction.InputAction)
	{
	case InputActionType::UnknownInput:
		break;
	case InputActionType::Button:
		_InputAction.ButtonAction();
		break;
	case InputActionType::Axis:
		//_InputAction.AxisAction();
		break;
	default:
		break;
	}
}
