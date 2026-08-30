#include "SceneManager.h"


void SceneManager::Init()
{
	AddScene("DefaultScene", new Scene());
	ChangeCurrentScene("DefaultScene");
}

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
	
}

void SceneManager::Cleanup()
{
	for (auto& pair : m_SceneList)
	{
		delete pair.second;
	}
	m_SceneList.clear();
	m_CurrentScene = nullptr;
}

Scene* SceneManager::GetSpecificScene(const std::string& _Name)
{
	if (m_SceneList.count(_Name) > 0)
	{
		return m_SceneList[_Name];
	}
	return nullptr;
}

void SceneManager::ChangeCurrentScene(const std::string& _Name)
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->UnloadScene();
	}

	if (m_SceneList.count(_Name) > 0)
	{
		m_CurrentScene = m_SceneList[_Name];
		m_CurrentScene->LoadScene();
		m_CurrentScene->Init();
	}
}

bool SceneManager::AddScene(const std::string& _Name, Scene* _Scene)
{
	if (m_SceneList.count(_Name) == 0)
	{
		m_SceneList[_Name] = _Scene;
		return true;
	}
	return false;
}

void SceneManager::UpdateCurrentScene(float _DeltaTime)
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->Update(_DeltaTime);
	}
}
