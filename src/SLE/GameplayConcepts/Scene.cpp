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
		gameObject->DrawGameObject(_FrameInfo);
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
}

void Scene::ClearAllGameObjects()
{
	for(GameObjectBase* gameObject : m_GameObjectList)
	{
		delete gameObject;
	}
	m_GameObjectList.clear();
}

