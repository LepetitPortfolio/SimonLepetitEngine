#pragma once
#include <vector>
#include <iostream>



class GameObjectComponentBase
{
public:
	virtual ~GameObjectComponentBase() = 0;

	void Enabled(bool _Enabled);
	virtual void Init() = 0;
	virtual void Desinit() = 0;
	virtual void Update() = 0;

protected:

	bool m_Enabled = false;

};

inline GameObjectComponentBase::~GameObjectComponentBase() {}