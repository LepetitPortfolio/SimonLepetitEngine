#pragma once
#include "GameObjectBase.h"

#include <vector>


class Scene
{
public:
	Scene();
	~Scene();

	void Init();

	void LoadScene();
	void UnloadScene();

	void Update();

	void AddGameObject(GameObjectBase* _GameObject);
	void AddGameObject(class Model* _Model);
	void RemoveGameObject(GameObjectBase* _GameObject);

	void ClearAllGameObjects();

private:

	std::vector<GameObjectBase*> m_GameObjectList;

};
