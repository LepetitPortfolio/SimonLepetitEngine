#pragma once
#include <functional>
#include <vector>
#include <algorithm>

template<typename... Args>
class DelegateMulticast {
public:
	using FuncType = std::function<void(Args...)>;

	void Clear() 
	{
		m_FunctionsDelegate.clear();
	}

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

	void operator()(Args... args) const 
	{
		Execute(args...);
	}

	void operator+=(const FuncType& function)
	{
		m_FunctionsDelegate.push_back(function);
	}

	// ⚠️ Limité aux fonctions/lambda sans capture ou pointeurs de fonction
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
	std::vector<FuncType> m_FunctionsDelegate;
};