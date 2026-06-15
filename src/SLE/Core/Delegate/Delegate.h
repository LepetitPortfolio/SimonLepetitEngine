#pragma once
#include <functional>

template<class Ret, typename... Args>
class Delegate
{
public:
	using FuncType = std::function<Ret(Args...)>;

	void Bind(const FuncType& _Function)
	{
		m_functionDelegate = _Function;
	}

	void Clear() 
	{
		m_functionDelegate = nullptr;
	}

	Ret Execute(Args... _Args) const
	{
		if(!m_functionDelegate)
		{
			return Ret();
		}
		return m_functionDelegate(_Args...);
	}

	// Appelle toutes les fonctions liées
	Ret operator()(Args... _Args) const
	{
		return Execute(_Args...);
	}

	/*
	* Adds an function or a methode
	*
	*/
	void operator=(const FuncType& _Function)
	{
		Bind(_Function);
	}

private:
	FuncType m_functionDelegate;
};