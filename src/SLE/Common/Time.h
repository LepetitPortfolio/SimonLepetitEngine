#pragma once
#include <chrono>
#include <cstdint>

class Time
{
public:

	Time() = default;

	/**
	* Constructeur de conversion depuis une durée std::chrono.
	*
	* @tparam Rep Type représentant le type de la valeur de la durée (ex: int, float, long long).
	* @tparam Period Type représentant la période de la durée (ex: std::ratio<1>, std::milli, std::micro).
	* @param _Duration Durée std::chrono à convertir en objet Time.
	*
	* @details
	* Ce constructeur permet d'initialiser un objet `Time` à partir de n'importe quelle durée
	* définie par `std::chrono::duration<Rep, Period>`. Il stocke simplement la durée passée en paramètre
	* dans le membre `m_Duration` de la classe `Time`.
	*
	* @note
	* Ce constructeur est marqué `inline` pour éviter les surcoûts d'appel de fonction.
	* Il est également implicite, ce qui permet une conversion automatique depuis `std::chrono::duration`.
	*/
	template<typename Rep, typename Period>
	Time(const std::chrono::duration<Rep, Period>& _Duration);


	/**
	* Convertit la durée en secondes (float).
	* @return Durée en secondes (précision flottante).
	*
	* @details
	* Utilise `std::chrono::duration<float>` pour convertir la durée interne (en microsecondes) en secondes.
	*/
	float GetTimeInSeconds() const;

	/**
	* Convertit la durée en millisecondes (entier 32 bits).
	* @return Durée en millisecondes.
	*
	* @details
	* Utilise `std::chrono::duration_cast` pour convertir la durée en millisecondes.
	*/
	std::int32_t  GetTimeInMilliseconds() const;

	/**
	* Convertit la durée en microsecondes (entier 64 bits).
	* @return Durée en microsecondes.
	*
	* @details
	* Retourne directement le compteur de la durée interne, qui est en microsecondes.
	*/
	std::int64_t GetTimeInMicroseconds() const;

	/**
	* Retourne la durée interne sous forme de `std::chrono::microseconds`.
	* @return Durée en microsecondes (type `std::chrono::microseconds`).
	*/
	std::chrono::microseconds GetDuration() const;

	/**
	* Opérateur de conversion vers une durée std::chrono.
	*
	* @tparam Rep Type représentant le type de la valeur de la durée (ex: int, float, long long).
	* @tparam Period Type représentant la période de la durée (ex: std::ratio<1>, std::milli, std::micro).
	* @return std::chrono::duration<Rep, Period> Durée std::chrono équivalente à l'objet Time.
	*
	* @details
	* Cet opérateur permet de convertir un objet `Time` vers n'importe quelle durée `std::chrono::duration<Rep, Period>`.
	* Il retourne simplement le membre `m_Duration` de la classe `Time`, qui est déjà de type `std::chrono::duration`.
	*
	* @note
	* - Cet opérateur est marqué `inline` pour éviter les surcoûts d'appel de fonction.
	* - Il est également implicite, ce qui permet une conversion automatique vers `std::chrono::duration`.
	* - La conversion est **générique** : elle fonctionne pour n'importe quel type `Rep` et `Period`.
	*/
	template<typename Rep, typename Period>
	operator std::chrono::duration<Rep, Period>() const;

	/**
	* Variable statique représentant une durée nulle (0 microsecondes).
	* Initialisée en ligne pour éviter les problèmes de définition multiple.
	*/
	static const Time m_ZeroTime;

private:

	std::chrono::microseconds m_Duration{};
};

/**
 * Crée un objet `Time` à partir d'une durée en secondes (float).
 * @param _Amount Durée en secondes.
 * @return Objet `Time` correspondant.
 *
 * @details
 * Convertit la durée en secondes en microsecondes pour le stockage interne.
 */
Time Seconds(float _Amount);

/**
 * Crée un objet `Time` à partir d'une durée en millisecondes.
 * @param _Amount Durée en millisecondes.
 * @return Objet `Time` correspondant.
 */
Time Milliseconds(std::int32_t _Amount);

/**
 * Crée un objet `Time` à partir d'une durée en microsecondes.
 * @param _Amount Durée en microsecondes.
 * @return Objet `Time` correspondant.
 */
Time Microseconds(std::int64_t _Amount);

/**
 * Vérifie si deux durées sont égales.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return true si les durées sont égales, false sinon.
 */
bool operator==(Time _Left, Time _Right);

/**
 * Vérifie si deux durées sont différentes.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return true si les durées sont différentes, false sinon.
 */
bool operator!=(Time _Left, Time _Right);

/**
 * Vérifie si une durée est strictement inférieure à une autre.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return true si _Left < _Right, false sinon.
 */
bool operator<(Time _Left, Time _Right);

/**
 * Vérifie si une durée est inférieure ou égale à une autre.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return true si _Left <= _Right, false sinon.
 */
bool operator<=(Time _Left, Time _Right);

/**
 * Vérifie si une durée est strictement supérieure à une autre.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return true si _Left > _Right, false sinon.
 */
bool operator>(Time _Left, Time _Right);

/**
 * Vérifie si une durée est supérieure ou égale à une autre.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return true si _Left >= _Right, false sinon.
 */
bool operator>=(Time _Left, Time _Right);

/**
 * Additionne deux durées.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return Nouvelle durée égale à la somme des deux durées.
 */
Time operator+(Time _Left, Time _Right);

/**
 * Soustrait deux durées.
 * @param _Left Première durée.
 * @param _Right Deuxième durée.
 * @return Nouvelle durée égale à la différence des deux durées.
 */
Time operator-(Time _Left, Time _Right);

/**
 * Ajoute une durée à une autre (version avec affectation).
 * @param _Left Durée à modifier.
 * @param _Right Durée à ajouter.
 * @return Référence vers la durée modifiée.
 *
 * @note Cette fonction semble contenir une erreur : elle retourne `_Left` par valeur au lieu de `Time&`.
 * Elle devrait probablement être : `Time& operator+=(Time& _Left, Time _Right)`.
 */
Time& operator+=(Time _Left, Time _Right);

/**
 * Soustrait une durée à une autre (version avec affectation).
 * @param _Left Durée à modifier.
 * @param _Right Durée à soustraire.
 * @return Référence vers la durée modifiée.
 *
 * @note Cette fonction semble contenir une erreur : elle retourne `_Left` par valeur au lieu de `Time&`.
 * Elle devrait probablement être : `Time& operator-=(Time& _Left, Time _Right)`.
 */
Time& operator-=(Time _Left, Time _Right);

/**
 * Multiplie une durée par un facteur flottant.
 * @param _Left Durée à multiplier.
 * @param _Right Facteur de multiplication.
 * @return Nouvelle durée égale à _Left * _Right.
 */
Time operator*(Time _Left, float _Right);

/**
 * Multiplie un facteur flottant par une durée.
 * @param _Left Facteur de multiplication.
 * @param _Right Durée à multiplier.
 * @return Nouvelle durée égale à _Left * _Right.
 */
Time operator*(float _Left, Time _Right);

/**
 * Multiplie une durée par un facteur entier (64 bits).
 * @param _Left Durée à multiplier.
 * @param _Right Facteur de multiplication.
 * @return Nouvelle durée égale à _Left * _Right.
 */
Time operator*(Time _Left, int64_t _Right);

/**
 * Multiplie un facteur entier (64 bits) par une durée.
 * @param _Left Facteur de multiplication.
 * @param _Right Durée à multiplier.
 * @return Nouvelle durée égale à _Left * _Right.
 */
Time operator*(int64_t _Left, Time _Right);

/**
 * Multiplie une durée par un facteur flottant (version avec affectation).
 * @param _Left Durée à modifier.
 * @param _Right Facteur de multiplication.
 * @return Référence vers la durée modifiée.
 *
 * @note Cette fonction semble contenir une erreur : elle retourne `_Left` par valeur au lieu de `Time&`.
 * Elle devrait probablement être : `Time& operator*=(Time& _Left, float _Right)`.
 */
Time& operator*=(Time _Left, float _Right);

/**
 * Multiplie une durée par un facteur entier (64 bits) (version avec affectation).
 * @param _Left Durée à modifier.
 * @param _Right Facteur de multiplication.
 * @return Référence vers la durée modifiée.
 *
 * @note Cette fonction semble contenir une erreur : elle retourne `_Left` par valeur au lieu de `Time&`.
 * Elle devrait probablement être : `Time& operator*=(Time& _Left, int64_t _Right)`.
 */
Time& operator*=(Time _Left, int64_t _Right);

/**
 * Divise une durée par un facteur flottant.
 * @param _Left Durée à diviser.
 * @param _Right Facteur de division.
 * @return Nouvelle durée égale à _Left / _Right.
 * @throws assert Si _Right est égal à 0 (division par zéro).
 */
Time operator/(Time _Left, float _Right);

/**
 * Divise un facteur flottant par une durée.
 * @param _Left Facteur de division.
 * @param _Right Durée à diviser.
 * @return Nouvelle durée égale à _Left / _Right.
 * @throws assert Si _Right est égal à 0 (division par zéro).
 */
Time operator/(float _Left, Time _Right);

/**
 * Divise une durée par un facteur entier (64 bits).
 * @param _Left Durée à diviser.
 * @param _Right Facteur de division.
 * @return Nouvelle durée égale à _Left / _Right.
 * @throws assert Si _Right est égal à 0 (division par zéro).
 */
Time operator/(Time _Left, int64_t _Right);

/**
 * Divise un facteur entier (64 bits) par une durée.
 * @param _Left Facteur de division.
 * @param _Right Durée à diviser.
 * @return Nouvelle durée égale à _Left / _Right.
 * @throws assert Si _Right est égal à 0 (division par zéro).
 */
Time operator/(int64_t _Left, Time _Right);

/**
 * Divise une durée par un facteur flottant (version avec affectation).
 * @param _Left Durée à modifier.
 * @param _Right Facteur de division.
 * @return Référence vers la durée modifiée.
 * @throws assert Si _Right est égal à 0 (division par zéro).
 *
 * @note Cette fonction semble contenir une erreur : elle retourne `_Left` par valeur au lieu de `Time&`.
 * Elle devrait probablement être : `Time& operator/=(Time& _Left, float _Right)`.
 */
Time& operator/=(Time _Left, float _Right);

/**
 * Divise une durée par un facteur entier (64 bits) (version avec affectation).
 * @param _Left Durée à modifier.
 * @param _Right Facteur de division.
 * @return Référence vers la durée modifiée.
 * @throws assert Si _Right est égal à 0 (division par zéro).
 *
 * @note Cette fonction semble contenir une erreur : elle retourne `_Left` par valeur au lieu de `Time&`.
 * Elle devrait probablement être : `Time& operator/=(Time& _Left, int64_t _Right)`.
 */
Time& operator/=(Time _Left, int64_t _Right);

/**
 * Divise deux durées et retourne le résultat sous forme de float (rapport).
 * @param _Left Durée dividende.
 * @param _Right Durée diviseur.
 * @return Rapport des deux durées (en secondes).
 * @throws assert Si _Right est égal à 0 (division par zéro).
 */
float operator/(Time _Left, Time _Right);

/**
 * Calcule le modulo de deux durées.
 * @param _Left Durée dividende.
 * @param _Right Durée diviseur.
 * @return Reste de la division de _Left par _Right (en microsecondes).
 * @throws assert Si _Right est égal à 0 (modulo par zéro).
 */
Time operator%(Time _Left, Time _Right);

/**
 * Calcule le modulo de deux durées (version avec affectation).
 * @param _Left Durée à modifier.
 * @param _Right Durée diviseur.
 * @return Référence vers la durée modifiée.
 * @throws assert Si _Right est égal à 0 (modulo par zéro).
 *
 * @note Cette fonction semble contenir une erreur : elle retourne `_Left` par valeur au lieu de `Time&`.
 * Elle devrait probablement être : `Time& operator%=(Time& _Left, Time _Right)`.
 */
Time& operator%=(Time _Left, Time _Right);

#include "Time.inl"