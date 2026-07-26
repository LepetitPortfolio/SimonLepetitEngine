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
	void UpdateDraw(VulkanFrameInfo _FrameInfo);

	void AddGameObject(GameObjectBase* _GameObject);
	void AddGameObject(class Mesh* _Mesh);
	void RemoveGameObject(GameObjectBase* _GameObject);

	void ClearAllGameObjects();

private:

	std::vector<GameObjectBase*> m_GameObjectList;

};
