#pragma once

#include "Scene.h"

#include <unordered_map>

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	Scene* GetCurrentScene() { return m_CurrentScene; }
	Scene* GetSpecificScene(const std::string& _Name);

	void Init();
	void Cleanup();

	void ChangeCurrentScene(const std::string& _Name);

	bool AddScene(const std::string& _Name, Scene* _Scene);

	void UpdateCurrentScene();

private:

	Scene* m_CurrentScene;

	std::unordered_map<std::string, Scene*> m_SceneList;
};


