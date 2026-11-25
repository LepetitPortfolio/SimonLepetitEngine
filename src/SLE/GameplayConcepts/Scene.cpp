#include "Scene.h"
#include "../Graphics/Mesh.h"
#include "GameObject/GameObject.h"
#include "GameObjectComponents/MeshComponent.h"

Scene::Scene()
{
	m_GameObjectList = std::vector<GameObjectBase*>();
}

Scene::~Scene()
{
	// Clear all game objects
	ClearAllGameObjects();
}

void Scene::Init()
{
}

void Scene::LoadScene()
{
}

void Scene::UnloadScene()
{
}

void Scene::Update(float _DeltaTime)
{

}

void Scene::UpdateDraw(VulkanFrameInfo& _FrameInfo)
{
	for (GameObjectBase* gameObject : m_GameObjectList)
	{
		if (gameObject->IsEnabled())
		{
			gameObject->DrawGameObject(_FrameInfo);
		}
	}
}


void Scene::AddGameObject(GameObjectBase* _GameObject)
{
	m_GameObjectList.push_back(_GameObject);
}

void Scene::AddGameObject(Mesh* _Mesh)
{
	GameObjectBase* gameObject = new GameObject();

	gameObject->AddComponent(new MeshComponent(_Mesh));
	AddGameObject(gameObject);
}

void Scene::RemoveGameObject(GameObjectBase* _GameObject)
{
	auto it = std::find(m_GameObjectList.begin(), m_GameObjectList.end(), _GameObject);
	if (it != m_GameObjectList.end())
	{
		delete* it;
		m_GameObjectList.erase(it);
	}
}

void Scene::ClearAllGameObjects()
{
	for(GameObjectBase* gameObject : m_GameObjectList)
	{
		delete gameObject;
	}
	m_GameObjectList.clear();
}

