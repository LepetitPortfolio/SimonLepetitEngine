#pragma once

#include <chrono>
#include <ratio>
#include <type_traits>

using ClockType = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

static_assert(ClockType::is_steady, "Provided implementation is not a monotonic clock");
static_assert(std::ratio_less_equal_v<ClockType::period, std::micro >, "Clock resolution is too low. Expecting at least a microsecond precision");


class Time;

class Clock
{
public:

	/**
	 * Relance le chronomètre s'il est arrêté.
	 *
	 * Si le chronomètre n'est pas en cours d'exécution (IsRunning() == false) :
	 * - Ajoute à m_RefPoint la durée écoulée entre l'arrêt (m_StopPoint) et maintenant (ClockType::now()).
	 *   Cela permet de reprendre la mesure du temps là où elle s'était arrêtée.
	 * - Réinitialise m_StopPoint à une valeur vide (time_point vide), indiquant que le chronomètre est en cours.
	 */
	void Start();

	/**
	 * Arrête le chronomètre s'il est en cours.
	 *
	 * Si le chronomètre est en cours d'exécution (IsRunning() == true) :
	 * - Enregistre l'heure actuelle (ClockType::now()) dans m_StopPoint pour marquer l'arrêt.
	 */
	void Stop();

	/**
	* @fn Time Clock::Restart()
	* @brief Redémarre le chronomètre et retourne le temps écoulé depuis le dernier démarrage.
	*
	* - Récupère le temps écoulé actuel (GetElapsedTime()) avant de redémarrer.
	* - Réinitialise m_RefPoint à l'heure actuelle (ClockType::now()) pour commencer une nouvelle mesure.
	* - Réinitialise m_StopPoint à une valeur vide (time_point vide).
	* - @return Le temps écoulé (en microsecondes) depuis le dernier démarrage.
	*/
	Time Restart();

	/**
	* Réinitialise le chronomètre et retourne le temps écoulé depuis le dernier démarrage.
	*
	* - Récupère le temps écoulé actuel (GetElapsedTime()) avant la réinitialisation.
	* - Réinitialise m_RefPoint à l'heure actuelle (ClockType::now()).
	* - Définit m_StopPoint à m_RefPoint, ce qui force IsRunning() à retourner false (car m_StopPoint != time_point vide).
	* - @return Le temps écoulé (en microsecondes) depuis le dernier démarrage.
	*/
	Time Reset();

	/**
	* Vérifie si le chronomètre est en cours d'exécution.
	*
	* - @return true si m_StopPoint est vide (time_point vide), ce qui signifie que le chronomètre est actif.
	* - @return false si m_StopPoint contient une valeur, indiquant que le chronomètre est arrêté.
	*/
	bool IsRunning() const;

	/**
	* @fn Time Clock::GetElapsedTime() const
	* @brief Calcule et retourne le temps écoulé depuis le dernier démarrage (en microsecondes).
	*
	* - Si le chronomètre est en cours (IsRunning() == true) :
	*   - Retourne la durée entre l'heure actuelle (ClockType::now()) et m_RefPoint.
	* - Si le chronomètre est arrêté (IsRunning() == false) :
	*   - Retourne la durée entre m_StopPoint et m_RefPoint.
	* - @return Le temps écoulé en microsecondes (std::chrono::microseconds).
	*/
	Time GetElapsedTime() const;

private:

	/**
	 * Point de référence pour le début de la mesure du temps.
	 * Initialisé à l'heure actuelle (ClockType::now()) lors de la construction.
	 * Représente le moment à partir duquel le temps écoulé est calculé.
	 */
	ClockType::time_point m_RefPoint{ ClockType::now() };

	/**
	 * Point d'arrêt temporaire du chronomètre.
	 * - Si vide (time_point vide), le chronomètre est en cours d'exécution (IsRunning() == true).
	 * - Si non vide, contient l'heure à laquelle le chronomètre a été arrêté (Stop() appelé).
	 */
	ClockType::time_point m_StopPoint;

};