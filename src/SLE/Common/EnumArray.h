#pragma once

#include <array>
#include <type_traits>

#include <cassert>
#include <cstddef>


/**
 * Structure héritant de std::array<Value, Count> pour permettre un accès indexé via des valeurs d'énumération (Enum).
 * @tparam Enum Type énuméré utilisé comme clé pour accéder aux éléments du tableau.
 * @tparam Value Type des valeurs stockées dans le tableau.
 * @tparam Count Nombre d'éléments dans le tableau.
 *
 * Cette structure permet d'utiliser des valeurs d'énumération comme indices pour accéder aux éléments du tableau,
 * tout en garantissant la sécurité des accès via des assertions.
 */
template<typename Enum, typename Value, size_t Count>
struct EnumArray : public std::array<Value, Count>
{
	/**
	 * Vérification statique : le premier paramètre de template doit être un type énuméré.
	 */
	static_assert(std::is_enum_v<Enum>, "EnumArray requires an enum type as the first template parameter.");

	/**
	 * Opérateur d'accès non-const pour accéder à un élément du tableau via une valeur d'énumération.
	 *
	 * @param _Key Valeur de l'énumération utilisée comme index.
	 * @return Référence vers l'élément du tableau correspondant à l'index de l'énumération.
	 *
	 * @details
	 * - Convertit la valeur de l'énumération (_Key) en un index de type std::size_t.
	 * - Vérifie que l'index est valide (inférieur à Count) via une assertion.
	 * - Utilise l'opérateur [] de std::array pour accéder à l'élément.
	 */
	constexpr Value& operator[](Enum _Key)
	{
		const auto index = static_cast<std::size_t>(_Key);
		assert(index < Count && "Enum value out of bounds.");
		return std::array<Value, Count>::operator[](index);
	}

	/**
	 * Opérateur d'accès const pour accéder à un élément du tableau via une valeur d'énumération.
	 *
	 * @param _Key Valeur de l'énumération utilisée comme index.
	 * @return Référence constante vers l'élément du tableau correspondant à l'index de l'énumération.
	 *
	 * @details
	 * - Identique à l'opérateur non-const, mais retourne une référence constante pour garantir l'immuabilité.
	 * - Convertit la valeur de l'énumération (_Key) en un index de type std::size_t.
	 * - Vérifie que l'index est valide (inférieur à Count) via une assertion.
	 * - Utilise l'opérateur [] de std::array pour accéder à l'élément.
	 */
	constexpr const Value& operator[](Enum _Key) const
	{
		const auto index = static_cast<std::size_t>(_Key);
		assert(index < Count && "Enum value out of bounds.");
		return std::array<Value, Count>::operator[](index);
	}
};