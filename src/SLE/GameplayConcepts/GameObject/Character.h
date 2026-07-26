#pragma once
#include "GameObject.h"

class Character : public GameObject
{
public:
	Character();
	~Character();

	virtual void SetupInputComponent();
};