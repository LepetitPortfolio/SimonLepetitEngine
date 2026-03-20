#pragma once

#include <array>
#include <type_traits>

#include <cassert>
#include <cstddef>

template<typename Enum, typename Value, size_t Count>
struct EnumArray : public std::array<Value, Count>
{
	static_assert(std::is_enum_v<Enum>, "EnumArray requires an enum type as the first template parameter.");

	constexpr Value& operator[](Enum _Key)
	{
		const auto index = static_cast<std::size_t>(_Key);
		assert(index < Count && "Enum value out of bounds.");
		return std::array<Value, Count>::operator[](index);
	}

	constexpr const Value& operator[](Enum _Key) const
	{
		const auto index = static_cast<std::size_t>(_Key);
		assert(index < Count && "Enum value out of bounds.");
		return std::array<Value, Count>::operator[](index);
	}
};