#pragma once
#include "../Inputs/Joystick.h"
#include "../Inputs/Keyboard.h"
#include "../Inputs/Mouse.h"
#include "../Inputs/Sensor_cls.h"

#include "../../Common/Vector.h"

#include <type_traits>
#include <variant>


class Event_cls
{
public: 
	struct Closed_str
	{
	};

	struct Resized_str
	{
		Vector2u Size;
	};

	struct FocusLost_str
	{
	};

	struct FocusGained_str
	{
	};

	struct TextEntered_str
	{
		char32_t Unicode{};
	};

	struct KeyPressed_str
	{
		Keyboard KeyCode{};
		ScanCode ScanCode{};
		bool Alt{};
		bool Control{};
		bool Shift{};
		bool System{};
	};

	struct KeyReleased_str
	{
		Keyboard KeyCode{};
		ScanCode ScanCode{};
		bool Alt{};
		bool Control{};
		bool Shift{};
		bool System{};
	};

	struct MouseWheelScrolled_str
	{
		MouseWheel_e Wheel{};
		float Delta{};
		Vector2i Position{};
	};

	struct MouseButtonPressed_str
	{
		MouseButton_e Button{};
		Vector2i Position{};
	};

	struct MouseButtonReleased_str
	{
		MouseButton_e Button{};
		Vector2i Position{};
	};

	struct MouseMoved_str
	{
		Vector2i Position{};
	};

	struct MouseMovedRaw_str
	{
		Vector2i Position{};
	};

	struct MouseEntered_str
	{
	};

	struct MouseLeft_str
	{
	};

	struct JoystickButtonPressed_str
	{
		unsigned int JoystickID{};
		unsigned int Button{};
	};

	struct JoystickButtonReleased_str
	{
		unsigned int JoystickID{};
		unsigned int Button{};
	};

	struct JoystickMoved_str
	{
		unsigned int JoystickID{};
		JoystickAxis_e Axis{};
		float Position{};
	};

	struct JoystickConnected_str
	{
		unsigned int JoystickID{};
	};

	struct JoystickDisconnected_str
	{
		unsigned int JoystickID{};
	};

	struct TouchBegan_str
	{
		unsigned int FingerID{};
		Vector2i Position{};
	};

	struct TouchMoved_str
	{
		unsigned int FingerID{};
		Vector2i Position{};
	};

	struct TouchEnded_str
	{
		unsigned int FingerID{};
		Vector2i Position{};
	};

	struct SensorChanged_str
	{
		SensorType_e Type{};
		Vector3f Value{};
	};

	template<typename TEventSubType>
	Event_cls(const TEventSubType& _EventSubType);

	template<typename TEventSubType>
	bool Is() const;

	template<typename TEventSubType>
	const TEventSubType& GetIf() const;

	template<typename Visitor>
	decltype(auto) Visit(Visitor&& _Visitor)const;

	private:

		std::variant < Closed_str,
					Resized_str,
					FocusLost_str,
					FocusGained_str,
					TextEntered_str,
					KeyPressed_str,
					KeyReleased_str,
					MouseWheelScrolled_str,
					MouseButtonPressed_str,
					MouseButtonReleased_str,
					MouseMoved_str,
					MouseMovedRaw_str,
					MouseEntered_str,
					MouseLeft_str,
					JoystickButtonPressed_str,
					JoystickButtonReleased_str,
					JoystickMoved_str,
					JoystickConnected_str,
					JoystickDisconnected_str,
					TouchBegan_str,
					TouchMoved_str,
					TouchEnded_str,
					SensorChanged_str > m_Data;

		template<typename T>
		static bool IsEventSubType = IsInParameterPack<T>(decltype (&m_Data)(nullptr));

		template<typename Handler>
		static bool IsEventHandler = IsInvocableWithEventSubType<Handler>(decltype (&m_Data)(nullptr));


		template <typename T, typename... Ts>
		static bool IsInParameterPack(const std::variant<Ts...>* _Variant)
		{
			return std::disjunction_v<std::is_same<T, Ts>...>;
		}

		friend class Window_cls;

		template <typename Handler, typename... Ts>
		static bool IsInvocableWithEventSubType(const std::variant<Ts...>* _Variant)
		{
			return std::disjunction_v<std::is_invocable<Handler&, Ts&>...>;
		}

		
};


