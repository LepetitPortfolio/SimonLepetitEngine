#pragma once
#include <vector>
#include <iostream>

class GameObjectBase;

class GameObjectComponentBase
{
public:
	GameObjectComponentBase() = default;
	virtual ~GameObjectComponentBase() = 0;

	uint64_t GetUID();

	void Enabled();
	void Enabled(bool _Enabled);
	virtual void Init(GameObjectBase* _GameObjectParent) = 0;
	virtual void Desinit() = 0;
	virtual void Update() = 0;

protected:

	GameObjectBase* m_GameObjectParent = nullptr;
	uint64_t m_UID = 0;
	bool m_Enabled = false;

	virtual void EnabledAction() = 0;
	virtual void DisableAction() = 0;

};

inline GameObjectComponentBase::~GameObjectComponentBase() {}