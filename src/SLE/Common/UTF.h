#pragma once

#include <array>
#include <locale>

#include <cstdint>
#include <cstdlib>

class UTF
{
public:

	/**
	* Décode un point de code UTF-8 à partir d'un itérateur.
	* @tparam T Type de l'itérateur (doit pointer vers des `char` ou `uint8_t`).
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @param _Output Référence vers le point de code décodé (sortie).
	* @param _Replacement Point de code de remplacement en cas d'erreur.
	* @return Itérateur pointant après le dernier octet du point de code décodé.
	*
	* @details
	* - Utilise un tableau `trailing` pour déterminer le nombre d'octets de continuation en fonction du premier octet.
	* - Utilise un tableau `offsets` pour ajuster le point de code final.
	* - Si le nombre d'octets de continuation est suffisant, décode le point de code en combinant les octets.
	* - Si le premier octet est invalide ou s'il n'y a pas assez d'octets de continuation, utilise `_Replacement`.
	*
	* @note
	* Les octets de continuation en UTF-8 commencent par `10xxxxxx`.
	* Le premier octet indique le nombre d'octets de continuation (ex: `110xxxxx` = 1 octet de continuation).
	*/
	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	/**
	* Avance l'itérateur vers le prochain point de code UTF (caractère suivant).
	* @tparam T Type de l'itérateur.
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @return Itérateur pointant vers le début du prochain point de code.
	*
	* @details
	* Utilise `Decode` pour lire un point de code UTF et avance l'itérateur en conséquence.
	* Le point de code lu est ignoré (non stocké).
	*/
	template<typename T>
	static T Next(T _Begin, T _End);

	/**
	* Compte le nombre de points de code UTF dans une plage.
	* @tparam T Type de l'itérateur.
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @return Nombre de points de code dans la plage.
	*
	* @details
	* Parcourt la plage en appelant `Next` jusqu'à atteindre la fin, en incrémentant un compteur à chaque itération.
	*/
	template<typename T>
	static std::size_t Count(T _Begin, T _End);

	/**
	* Convertit une chaîne ANSI en UTF-32.
	* @tparam In Type de l'itérateur d'entrée (ANSI).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne ANSI.
	* @param _End Itérateur de fin de la chaîne ANSI.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @param _Locale Locale utilisée pour la conversion.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Pour chaque caractère ANSI, utilise `UTF32::DecodeANSI` pour le convertir en UTF-32,
	* puis `Encode` pour l'écrire dans la sortie.
	*/
	template<typename In, typename Out>
	static Out FromANSI(In _Begin, In _End, Out _Output, std::locale& _Locale = {});

	/**
	* Convertit une chaîne WIDE (wchar_t) en UTF-32.
	* @tparam In Type de l'itérateur d'entrée (WIDE).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne WIDE.
	* @param _End Itérateur de fin de la chaîne WIDE.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Pour chaque caractère WIDE, utilise `UTF32::DecodeWIDE` pour le convertir en UTF-32,
	* puis `Encode` pour l'écrire dans la sortie.
	*/
	template<typename In, typename Out>
	static Out FromWIDE(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne LATIN1 en UTF-32.
	* @tparam In Type de l'itérateur d'entrée (LATIN1).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne LATIN1.
	* @param _End Itérateur de fin de la chaîne LATIN1.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* LATIN1 est un encodage 1:1 avec les 256 premiers points de code Unicode.
	* Utilise `Copy` pour copier directement les octets (pas de conversion nécessaire).
	*/
	template<typename In, typename Out>
	static Out FromLATIN1(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-32 en ANSI.
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (ANSI).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne ANSI.
	* @param _Replacement Caractère de remplacement en cas d'erreur.
	* @param _Locale Locale utilisée pour la conversion.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Pour chaque point de code UTF-32, utilise `Decode` pour le lire,
	* puis `UTF32::EncodeANSI` pour le convertir en ANSI.
	*/
	template<typename In, typename Out>
	static Out ToANSI(In _Begin, In _End, Out _Output, char _Replacement = 0, std::locale& _Locale = {});

	/**
	* Convertit une chaîne UTF-32 en WIDE (wchar_t).
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (WIDE).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne WIDE.
	* @param _Replacement Caractère de remplacement en cas d'erreur.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Pour chaque point de code UTF-32, utilise `Decode` pour le lire,
	* puis `UTF32::EncodeWIDE` pour le convertir en WIDE.
	*/
	template<typename In, typename Out>
	static Out ToWIDE(In _Begin, In _End, Out _Output, wchar_t _Replacement = 0);

	/**
	* Convertit une chaîne UTF-32 en LATIN1.
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (LATIN1).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne LATIN1.
	* @param _Replacement Caractère de remplacement si le point de code > 255.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Pour chaque point de code UTF-32, vérifie s'il est dans la plage LATIN1 (0-255).
	* Si oui, le copie tel quel, sinon utilise `_Replacement`.
	*/
	template<typename In, typename Out>
	static Out ToLATIN1(In _Begin, In _End, Out _Output, char _Replacement = 0);

	/**
	* Convertit une chaîne UTF-32 en UTF-8.
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (UTF-8).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne UTF-8.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Utilise `Copy` car UTF-32 et UTF-8 sont tous deux des encodages Unicode.
	*/
	template<typename In, typename Out>
	static Out ToUTF8(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-32 en UTF-16.
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (UTF-16).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne UTF-16.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*/
	template<typename In, typename Out>
	static Out ToUTF16(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-32 en UTF-32 (copie directe).
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*/
	template<typename In, typename Out>
	static Out ToUTF32(In _Begin, In _End, Out _Output);

protected:

	/**
	* Copie une plage de caractères d'un itérateur d'entrée vers un itérateur de sortie.
	* @tparam InIt Type de l'itérateur d'entrée.
	* @tparam OutIt Type de l'itérateur de sortie.
	* @param _First Itérateur de début de la plage d'entrée.
	* @param _Last Itérateur de fin de la plage d'entrée.
	* @param _Ouput Itérateur de sortie où copier les caractères.
	* @return Itérateur de sortie pointant après le dernier caractère copié.
	*
	* @details
	* Copie chaque caractère de la plage [_First, _Last) vers _Ouput en les castant vers le type de valeur du conteneur de sortie.
	*/
	template<typename InIt, typename OutIt>
	static OutIt Copy(InIt _First, InIt _Last, OutIt _Ouput);

};

class UTF8 : public UTF
{
public:


	/**
	* Décode un point de code UTF-8 à partir d'un itérateur.
	* @tparam T Type de l'itérateur (doit pointer vers des `char` ou `uint8_t`).
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @param _Output Référence vers le point de code décodé (sortie).
	* @param _Replacement Point de code de remplacement en cas d'erreur.
	* @return Itérateur pointant après le dernier octet du point de code décodé.
	*
	* @details
	* - Utilise un tableau `trailing` pour déterminer le nombre d'octets de continuation en fonction du premier octet.
	* - Utilise un tableau `offsets` pour ajuster le point de code final.
	* - Si le nombre d'octets de continuation est suffisant, décode le point de code en combinant les octets.
	* - Si le premier octet est invalide ou s'il n'y a pas assez d'octets de continuation, utilise `_Replacement`.
	*
	* @note
	* Les octets de continuation en UTF-8 commencent par `10xxxxxx`.
	* Le premier octet indique le nombre d'octets de continuation (ex: `110xxxxx` = 1 octet de continuation).
	*/
	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	/**
	* Encode un point de code UTF-32 en UTF-8.
	* @tparam T Type de l'itérateur de sortie.
	* @param _Input Point de code UTF-32 à encoder.
	* @param _Output Itérateur de sortie pour la chaîne UTF-8.
	* @param _Replacement Octet de remplacement en cas d'erreur.
	* @return Itérateur de sortie pointant après le dernier octet écrit.
	*
	* @details
	* - Vérifie si `_Input` est un point de code valide (0x0000 à 0x10FFFF, hors plage des surrogates 0xD800-0xDFFF).
	* - Détermine le nombre d'octets nécessaires pour encoder le point de code :
	*   - 1 octet : 0x0000-0x007F
	*   - 2 octets : 0x0080-0x07FF
	*   - 3 octets : 0x0800-0xFFFF
	*   - 4 octets : 0x10000-0x10FFFF
	* - Utilise un tableau `firstBytes` pour déterminer le premier octet de la séquence UTF-8.
	* - Extrait les bits du point de code et les répartit dans les octets de sortie.
	*
	* @note
	* Les octets de continuation en UTF-8 commencent par `10xxxxxx`.
	* Le premier octet commence par `110xxxxx` (2 octets), `1110xxxx` (3 octets), ou `11110xxx` (4 octets).
	*/
	template<typename T>
	static T Encode(char32_t _Input, T _Output, std::uint8_t _Replacement = 0);

	/**
	* Convertit une chaîne LATIN1 en UTF-8.
	* @tparam In Type de l'itérateur d'entrée (LATIN1).
	* @tparam Out Type de l'itérateur de sortie (UTF-8).
	* @param _Begin Itérateur de début de la chaîne LATIN1.
	* @param _End Itérateur de fin de la chaîne LATIN1.
	* @param _Output Itérateur de sortie pour la chaîne UTF-8.
	* @return Itérateur de sortie pointant après le dernier octet écrit.
	*
	* @details
	* LATIN1 est un encodage 1:1 avec les 256 premiers points de code Unicode.
	* Chaque caractère LATIN1 est encodé en UTF-8 (1 ou 2 octets).
	*/
	template<typename In, typename Out>
	static Out FromLATIN1(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-8 en LATIN1.
	* @tparam In Type de l'itérateur d'entrée (UTF-8).
	* @tparam Out Type de l'itérateur de sortie (LATIN1).
	* @param _Begin Itérateur de début de la chaîne UTF-8.
	* @param _End Itérateur de fin de la chaîne UTF-8.
	* @param _Output Itérateur de sortie pour la chaîne LATIN1.
	* @param _Replacement Caractère de remplacement si le point de code > 255.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Décode chaque point de code UTF-8 et vérifie s'il est dans la plage LATIN1 (0-255).
	* Si oui, le copie tel quel, sinon utilise `_Replacement`.
	*/
	template<typename In, typename Out>
	static Out ToLATIN1(In _Begin, In _End, Out _Output, char _Replacement = 0);

	/**
	* Convertit une chaîne UTF-8 en UTF-16.
	* @tparam In Type de l'itérateur d'entrée (UTF-8).
	* @tparam Out Type de l'itérateur de sortie (UTF-16).
	* @param _Begin Itérateur de début de la chaîne UTF-8.
	* @param _End Itérateur de fin de la chaîne UTF-8.
	* @param _Output Itérateur de sortie pour la chaîne UTF-16.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Décode chaque point de code UTF-8, puis l'encode en UTF-16.
	*/
	template<typename In, typename Out>
	static Out ToUTF16(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-8 en UTF-32.
	* @tparam In Type de l'itérateur d'entrée (UTF-8).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne UTF-8.
	* @param _End Itérateur de fin de la chaîne UTF-8.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Décode chaque point de code UTF-8 et l'écrit directement en UTF-32 (1:1).
	*/
	template<typename In, typename Out>
	static Out ToUTF32(In _Begin, In _End, Out _Output);
};

class UTF16 : public UTF
{
public:

	/**
	* Décode un point de code UTF-16 à partir d'un itérateur.
	* @tparam T Type de l'itérateur (doit pointer vers des `char16_t`).
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @param _Output Référence vers le point de code décodé (sortie).
	* @param _Replacement Point de code de remplacement en cas d'erreur.
	* @return Itérateur pointant après le dernier octet du point de code décodé.
	*
	* @details
	* - UTF-16 utilise des **paires de surrogates** pour représenter les points de code > 0xFFFF.
	* - Si le premier caractère est un **surrogate haut** (0xD800-0xDBFF), vérifie s'il est suivi d'un **surrogate bas** (0xDC00-0xDFFF).
	* - Si les deux surrogates sont valides, combine-les pour former le point de code final.
	* - Sinon, utilise `_Replacement`.
	*/
	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	/**
	* Encode un point de code UTF-32 en UTF-16.
	* @tparam T Type de l'itérateur de sortie.
	* @param _Input Point de code UTF-32 à encoder.
	* @param _Output Itérateur de sortie pour la chaîne UTF-16.
	* @param _Replacement Caractère de remplacement en cas d'erreur.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* - Si `_Input` est dans la **plage des surrogates** (0xD800-0xDFFF), utilise `_Replacement`.
	* - Si `_Input` est > 0x10FFFF, utilise `_Replacement`.
	* - Si `_Input` est <= 0xFFFF, l'écrit directement.
	* - Sinon, décompose `_Input` en une **paire de surrogates** (haut + bas).
	*/
	template<typename T>
	static T Encode(char32_t _Input, T _Output, std::uint16_t _Replacement = 0);

	/**
	* Convertit une chaîne UTF-16 en UTF-8.
	* @tparam In Type de l'itérateur d'entrée (UTF-16).
	* @tparam Out Type de l'itérateur de sortie (UTF-8).
	* @param _Begin Itérateur de début de la chaîne UTF-16.
	* @param _End Itérateur de fin de la chaîne UTF-16.
	* @param _Output Itérateur de sortie pour la chaîne UTF-8.
	* @return Itérateur de sortie pointant après le dernier octet écrit.
	*
	* @details
	* Décode chaque point de code UTF-16, puis l'encode en UTF-8.
	*/
	template<typename In, typename Out>
	static Out ToUTF8(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-16 en UTF-32.
	* @tparam In Type de l'itérateur d'entrée (UTF-16).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne UTF-16.
	* @param _End Itérateur de fin de la chaîne UTF-16.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Décode chaque point de code UTF-16 et l'écrit directement en UTF-32 (1:1).
	*/
	template<typename In, typename Out>
	static Out ToUTF32(In _Begin, In _End, Out _Output);
};

class UTF32 : public UTF
{
public:

	/**
	* Décode un point de code UTF-32 à partir d'un itérateur.
	* @tparam T Type de l'itérateur (doit pointer vers des `char32_t`).
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @param _Output Référence vers le point de code décodé (sortie).
	* @param _Replacement Point de code de remplacement (non utilisé ici).
	* @return Itérateur pointant après le point de code décodé.
	*
	* @details
	* UTF-32 est un encodage 1:1 : chaque point de code est stocké sur 4 octets.
	* Cette fonction se contente de lire le point de code et d'avancer l'itérateur.
	*/
	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	/**
	* Encode un point de code UTF-32.
	* @tparam T Type de l'itérateur de sortie.
	* @param _Input Point de code UTF-32 à encoder.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @param _Replacement Point de code de remplacement (non utilisé ici).
	* @return Itérateur de sortie pointant après le point de code écrit.
	*
	* @details
	* UTF-32 est un encodage 1:1 : chaque point de code est écrit tel quel.
	*/
	template<typename T>
	static T Encode(char32_t _Input, T _Output, std::uint32_t _Replacement = 0);

	/**
	* Avance l'itérateur vers le prochain point de code UTF-32.
	* @tparam T Type de l'itérateur.
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @return Itérateur pointant vers le prochain point de code.
	*/
	template<typename T>
	static T Next(T _Begin, T _End);

	/**
	* Compte le nombre de points de code UTF-32 dans une plage.
	* @tparam T Type de l'itérateur.
	* @param _Begin Itérateur de début.
	* @param _End Itérateur de fin.
	* @return Nombre de points de code dans la plage.
	*/
	template<typename T>
	static std::size_t Count(T _Begin, T _End);

	/**
	* Convertit une chaîne ANSI en UTF-32.
	* @tparam In Type de l'itérateur d'entrée (ANSI).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne ANSI.
	* @param _End Itérateur de fin de la chaîne ANSI.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @param _Locale Locale utilisée pour la conversion.
	* @return Itérateur de sortie pointant après le dernier point de code écrit.
	*
	* @details
	* Utilise `DecodeANSI` pour convertir chaque caractère ANSI en UTF-32.
	*/
	template<typename In, typename Out>
	static Out FromANSI(In _Begin, In _End, Out _Output, const std::locale& _Locale = {});

	/**
	* Convertit une chaîne WIDE (wchar_t) en UTF-32.
	* @tparam In Type de l'itérateur d'entrée (WIDE).
	* @tparam Out Type de l'itérateur de sortie (UTF-32).
	* @param _Begin Itérateur de début de la chaîne WIDE.
	* @param _End Itérateur de fin de la chaîne WIDE.
	* @param _Output Itérateur de sortie pour la chaîne UTF-32.
	* @return Itérateur de sortie pointant après le dernier point de code écrit.
	*
	* @details
	* Utilise `DecodeWIDE` pour convertir chaque caractère WIDE en UTF-32.
	*/
	template<typename In, typename Out>
	static Out FromWIDE(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-32 en ANSI.
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (ANSI).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne ANSI.
	* @param _Replacement Caractère de remplacement en cas d'erreur.
	* @param _Locale Locale utilisée pour la conversion.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Utilise `EncodeANSI` pour convertir chaque point de code UTF-32 en ANSI.
	*/
	template<typename In, typename Out>
	static Out ToANSI(In _Begin, In _End, Out _Output, char _Replacement = 0, const std::locale& _Locale = {});

	/**
	* Convertit une chaîne UTF-32 en WIDE (wchar_t).
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (WIDE).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne WIDE.
	* @param _Replacement Caractère de remplacement en cas d'erreur.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Utilise `EncodeWIDE` pour convertir chaque point de code UTF-32 en WIDE.
	*/
	template<typename In, typename Out>
	static Out ToWIDE(In _Begin, In _End, Out _Output, wchar_t _Replacement = 0);

	/**
	* Convertit une chaîne UTF-32 en UTF-8.
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (UTF-8).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne UTF-8.
	* @return Itérateur de sortie pointant après le dernier octet écrit.
	*
	* @details
	* Utilise `UTF8::Encode` pour convertir chaque point de code UTF-32 en UTF-8.
	*/
	template<typename In, typename Out>
	static Out ToUTF8(In _Begin, In _End, Out _Output);

	/**
	* Convertit une chaîne UTF-32 en UTF-16.
	* @tparam In Type de l'itérateur d'entrée (UTF-32).
	* @tparam Out Type de l'itérateur de sortie (UTF-16).
	* @param _Begin Itérateur de début de la chaîne UTF-32.
	* @param _End Itérateur de fin de la chaîne UTF-32.
	* @param _Output Itérateur de sortie pour la chaîne UTF-16.
	* @return Itérateur de sortie pointant après le dernier caractère écrit.
	*
	* @details
	* Utilise `UTF16::Encode` pour convertir chaque point de code UTF-32 en UTF-16.
	*/
	template<typename In, typename Out>
	static Out ToUTF16(In _Begin, In _End, Out _Output);

	/**
	* Décode un caractère ANSI en UTF-32 en utilisant une locale.
	* @tparam T Type du caractère ANSI (généralement `char`).
	* @param _Input Caractère ANSI à décoder.
	* @param _Locale Locale utilisée pour la conversion.
	* @return Point de code UTF-32 correspondant.
	*
	* @details
	* Utilise `std::use_facet<std::ctype<wchar_t>>` pour obtenir la facette de conversion de la locale,
	* puis `facet.widen()` pour convertir le caractère ANSI en `wchar_t`, puis en `char32_t`.
	*/
	template<typename T>
	static char32_t DecodeANSI(T _Input, const std::locale& _Locale = {});

	/**
	* Décode un caractère WIDE (wchar_t) en UTF-32.
	* @tparam T Type du caractère WIDE (généralement `wchar_t`).
	* @param _Input Caractère WIDE à décoder.
	* @return Point de code UTF-32 correspondant.
	*
	* @details
	* Sur la plupart des systèmes, `wchar_t` est déjà compatible avec UTF-32 (4 octets).
	* Cette fonction se contente de caster le caractère en `char32_t`.
	*/
	template<typename T>
	static char32_t DecodeWIDE(T _Input);

	/**
	* Encode un point de code UTF-32 en ANSI en utilisant une locale.
	* @tparam T Type de l'itérateur de sortie (ANSI).
	* @param _Codepoint Point de code UTF-32 à encoder.
	* @param _Output Itérateur de sortie pour la chaîne ANSI.
	* @param _Remplacement Caractère de remplacement en cas d'erreur.
	* @param _Locale Locale utilisée pour la conversion.
	* @return Itérateur de sortie pointant après le caractère écrit.
	*
	* @details
	* Utilise `std::use_facet<std::ctype<wchar_t>>` pour obtenir la facette de conversion de la locale,
	* puis `facet.narrow()` pour convertir le point de code en `char`.
	* Si le point de code ne peut pas être représenté en ANSI, utilise `_Remplacement`.
	*/
	template<typename T>
	static T EncodeANSI(char32_t _Codepoint, T _Output, char _Remplacement = 0, const std::locale& _Locale = {});

	/**
	* Encode un point de code UTF-32 en WIDE (wchar_t).
	* @tparam T Type de l'itérateur de sortie (WIDE).
	* @param _Codepoint Point de code UTF-32 à encoder.
	* @param _Output Itérateur de sortie pour la chaîne WIDE.
	* @param _Replacement Caractère de remplacement en cas d'erreur.
	* @return Itérateur de sortie pointant après le caractère écrit.
	*
	* @details
	* - Si `wchar_t` fait 4 octets (comme sur Linux), le point de code est écrit directement.
	* - Si `wchar_t` fait 2 octets (comme sur Windows), vérifie si le point de code est dans la plage valide pour UTF-16 (0x0000-0xD7FF ou 0xE000-0xFFFF).
	*   Si oui, l'écrit directement. Sinon, utilise `_Replacement`.
	*/
	template<typename T>
	static T EncodeWIDE(char32_t _Codepoint, T _Output, wchar_t _Replacement = 0);
};

#include "UTF.inl"