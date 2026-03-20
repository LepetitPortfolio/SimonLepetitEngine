
#pragma once
#include "../Inputs/Joystick.h"
#include "../Inputs/Keyboard.h"
#include "../Inputs/Mouse.h"
#include "../Inputs/Sensor.h"

#include "../../Common/Vector.h"

#include <type_traits>
#include <variant>


class Event
{
public: 
	struct Closed
	{
	};

	struct Resized
	{
		Vector2u Size;
	};

	struct FocusLost
	{
	};

	struct FocusGained
	{
	};

	struct TextEntered
	{
		char32_t Unicode{};
	};

	struct KeyPressed
	{
		Keyboard KeyCode{};
		ScanCode ScanCode{};
		bool Alt{};
		bool Control{};
		bool Shift{};
		bool System{};
	};

	struct KeyReleased
	{
		Keyboard KeyCode{};
		ScanCode ScanCode{};
		bool Alt{};
		bool Control{};
		bool Shift{};
		bool System{};
	};

	struct MouseWheelScrolled
	{
		MouseWheel Wheel{};
		float Delta{};
		Vector2i Position{};
	};

	struct MouseButtonPressed
	{
		MouseButton Button{};
		Vector2i Position{};
	};

	struct MouseButtonReleased
	{
		MouseButton Button{};
		Vector2i Position{};
	};

	struct MouseMoved
	{
		Vector2i Position{};
	};

	struct MouseMovedRaw
	{
		Vector2i Position{};
	};

	struct MouseEntered
	{
	};

	struct MouseLeft
	{
	};

	struct JoystickButtonPressed
	{
		unsigned int JoystickID{};
		unsigned int Button{};
	};

	struct JoystickButtonReleased
	{
		unsigned int JoystickID{};
		unsigned int Button{};
	};

	struct JoystickMoved
	{
		unsigned int JoystickID{};
		JoystickAxis Axis{};
		float Position{};
	};

	struct JoystickConnected
	{
		unsigned int JoystickID{};
	};

	struct JoystickDisconnected
	{
		unsigned int JoystickID{};
	};

	struct TouchBegan
	{
		unsigned int FingerID{};
		Vector2i Position{};
	};

	struct TouchMoved
	{
		unsigned int FingerID{};
		Vector2i Position{};
	};

	struct TouchEnded
	{
		unsigned int FingerID{};
		Vector2i Position{};
	};

	struct SensorChanged
	{
		SensorType Type{};
		Vector3f Value{};
	};

	template<typename TEventSubType>
	Event(const TEventSubType& _EventSubType);

	template<typename TEventSubType>
	bool Is() const;

	template<typename TEventSubType>
	const TEventSubType& GetIf() const;

	template<typename Visitor>
	decltype(auto) Visit(Visitor&& _Visitor)const;

	private:

		std::variant < Closed,
					Resized,
					FocusLost,
					FocusGained,
					TextEntered,
					KeyPressed,
					KeyReleased,
					MouseWheelScrolled,
					MouseButtonPressed,
					MouseButtonReleased,
					MouseMoved,
					MouseMovedRaw,
					MouseEntered,
					MouseLeft,
					JoystickButtonPressed,
					JoystickButtonReleased,
					JoystickMoved,
					JoystickConnected,
					JoystickDisconnected,
					TouchBegan,
					TouchMoved,
					TouchEnded,
					SensorChanged > m_Data;

		template<typename T>
		static bool IsEventSubType = IsInParameterPack<T>(decltype (&m_Data)(nullptr));

		template<typename Handler>
		static bool IsEventHandler = IsInvocableWithEventSubType<Handler>(decltype (&m_Data)(nullptr));


		template <typename T, typename... Ts>
		static bool IsInParameterPack(const std::variant<Ts...>* _Variant)
		{
			return std::disjunction_v<std::is_same<T, Ts>...>;
		}

		friend class Window;

		template <typename Handler, typename... Ts>
		static bool IsInvocableWithEventSubType(const std::variant<Ts...>* _Variant)
		{
			return std::disjunction_v<std::is_invocable<Handler&, Ts&>...>;
		}

		
};


