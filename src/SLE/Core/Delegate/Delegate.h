#pragma once
#include <functional>


template<class Ret, typename... Args>
class Delegate
{
public:

	/**
	* Type alias pour std::function avec la signature Ret(Args...).
	* Permet de définir le type de fonction que le Delegate peut stocker.
	*/
	using FuncType = std::function<Ret(Args...)>;

	/**
	* Lie une fonction au Delegate.
	* @param _Function Fonction à lier (de type FuncType).
	*
	* @details
	* Stocke la fonction `_Function` dans `m_functionDelegate`.
	* Si `_Function` est `nullptr`, le Delegate ne fera rien lors de l'appel.
	*/
	void Bind(const FuncType& _Function)
	{
		m_functionDelegate = _Function;
	}

	/**
	* Réinitialise le Delegate en supprimant la fonction liée.
	*
	* @details
	* Définit `m_functionDelegate` à `nullptr`, ce qui signifie que le Delegate ne fera rien lors de l'appel.
	*/
	void Clear() 
	{
		m_functionDelegate = nullptr;
	}

	/**
	* Exécute la fonction liée avec les arguments fournis.
	* @param _Args Arguments à passer à la fonction.
	* @return Résultat de type Ret. Si aucune fonction n'est liée, retourne une valeur par défaut de type Ret.
	*
	* @details
	* - Vérifie si `m_functionDelegate` est valide (non nul).
	* - Si `m_functionDelegate` est nul, retourne une valeur par défaut de type `Ret` (ex: `0` pour `int`, `false` pour `bool`, `nullptr` pour les pointeurs).
	* - Sinon, appelle `m_functionDelegate` avec les arguments `_Args...` et retourne le résultat.
	*/
	Ret Execute(Args... _Args) const
	{
		if(!m_functionDelegate)
		{
			return Ret();
		}
		return m_functionDelegate(_Args...);
	}

	/**
	* Opérateur d'appel de fonction. Permet d'utiliser le Delegate comme une fonction.
	* @param _Args Arguments à passer à la fonction.
	* @return Résultat de type Ret (identique à Execute).
	*
	* @details
	* Appelle `Execute(_Args...)` pour exécuter la fonction liée.
	* Cela permet d'utiliser le Delegate de manière transparente, comme une fonction normale.
	*/
	Ret operator()(Args... _Args) const
	{
		return Execute(_Args...);
	}

	/**
	* Opérateur d'affectation. Permet de lier une fonction au Delegate en utilisant l'opérateur `=`.
	* @param _Function Fonction à lier (de type FuncType).
	*
	* @details
	* Appelle `Bind(_Function)` pour lier la fonction au Delegate.
	* Cela permet une syntaxe plus naturelle pour lier une fonction.
	*/
	void operator=(const FuncType& _Function)
	{
		Bind(_Function);
	}

private:

	/**
	* Fonction stockée par le Delegate.
	* @details
	* `m_functionDelegate` est une instance de `std::function<Ret(Args...)>` qui stocke la fonction liée.
	* Si `m_functionDelegate` est `nullptr`, le Delegate ne fera rien lors de l'appel.
	*/
	FuncType m_functionDelegate;
};