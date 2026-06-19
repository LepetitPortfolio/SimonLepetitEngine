#pragma once
#include <functional>
#include <vector>
#include <algorithm>

template<typename... Args>
class DelegateMulticast {
public:

	/**
	* @brief Type alias pour std::function avec la signature void(Args...).
	* @details
	* Contrairement à `Delegate`, `DelegateMulticast` ne retourne rien (void) car il est conçu pour appeler plusieurs fonctions.
	* Si tu veux gérer des retours, il faudrait utiliser un autre mécanisme (ex: accumuler les résultats dans une liste).
	*/
	using FuncType = std::function<void(Args...)>;

	/**
	* @brief Supprime toutes les fonctions stockées dans le DelegateMulticast.
	*
	* @details
	* Appelle `clear()` sur le vecteur `m_FunctionsDelegate`, ce qui supprime toutes les fonctions enregistrées.
	* Après cet appel, `Execute` ou `operator()` ne feront rien.
	*/
	void Clear() 
	{
		m_FunctionsDelegate.clear();
	}

	/**
	* @brief Exécute toutes les fonctions stockées avec les arguments fournis.
	* @param args Arguments à passer à chaque fonction.
	*
	* @details
	* - **Copie locale** : Crée une copie locale de `m_FunctionsDelegate` pour éviter les problèmes si un callback modifie la liste pendant l'exécution (ex: en s'abonnant ou se désabonnant).
	* - **Boucle d'appel** : Parcourt chaque fonction dans la copie et l'appelle avec les arguments `args...`.
	* - **Gestion des erreurs** : Utilise un bloc `try-catch` pour capturer les exceptions levées par les callbacks. Cela évite qu'une erreur dans un callback n'interrompe l'exécution des autres.
	*   - **Note** : Actuellement, les erreurs sont simplement ignorées. Tu pourrais ajouter un système de logging ou de rappel d'erreur.
	*/
	void Execute(Args... args) const 
	{
		// Copie locale pour éviter les problèmes si un callback modifie la liste
		auto callbacksCopy = m_FunctionsDelegate;
		for (const auto& func : callbacksCopy) {
			try {
				func(args...);
			}
			catch (...) {
				// Gérer l'erreur ici (ex: log)
			}
		}
	}

	/**
	* @brief Opérateur d'appel de fonction. Permet d'utiliser le DelegateMulticast comme une fonction.
	* @param args Arguments à passer à chaque fonction.
	*
	* @details
	* Appelle `Execute(args...)` pour exécuter toutes les fonctions stockées.
	* Cela permet une syntaxe plus naturelle pour déclencher les callbacks.
	*/
	void operator()(Args... args) const 
	{
		Execute(args...);
	}

	/**
	* @brief Ajoute une fonction à la liste des callbacks.
	* @param function Fonction à ajouter (de type FuncType).
	*
	* @details
	* Ajoute la fonction `function` à la fin du vecteur `m_FunctionsDelegate`.
	* Cela permet de s'abonner à un événement en utilisant la syntaxe `+=`.
	*/
	void operator+=(const FuncType& function)
	{
		m_FunctionsDelegate.push_back(function);
	}

	/**
	* @brief Retire une fonction de la liste des callbacks.
	* @param function Fonction à retirer (de type FuncType).
	*
	* @details
	* - **Recherche de la fonction** : Utilise `std::find_if` pour trouver la fonction dans `m_FunctionsDelegate`.
	*   - Compare les **types cibles** (`target_type()`) et les **adresses cibles** (`target<void(Args...)>()`) des fonctions.
	*   - Cela permet de comparer les `std::function` entre elles, même si elles sont des lambdas ou des pointeurs de fonction.
	* - **Suppression** : Si la fonction est trouvée, elle est supprimée du vecteur avec `erase`.
	*/
	void operator-=(const FuncType& function) 
	{
		auto it = std::find_if(m_FunctionsDelegate.begin(), m_FunctionsDelegate.end(),
			[&function](const FuncType& f) {
				return f.target_type() == function.target_type() &&
					f.target<void(Args...)>() == function.target<void(Args...)>();
			});
		if (it != m_FunctionsDelegate.end()) {
			m_FunctionsDelegate.erase(it);
		}
	}

private:

	/**
	* @brief Vecteur de fonctions stockées.
	* @details
	* `m_FunctionsDelegate` est un conteneur qui stocke toutes les fonctions à appeler.
	* Chaque fonction est de type `FuncType` (std::function<void(Args...)>).
	*/
	std::vector<FuncType> m_FunctionsDelegate;
};