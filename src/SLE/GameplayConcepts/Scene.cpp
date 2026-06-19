#include "Scene.h"
#include "../Graphics/Models/Model.h"
#include "GameObject/GameObject.h"
#include "GameObjectComponents/ModelComponent.h"

Scene::Scene()
{
	m_GameObjectList = std::vector<GameObjectBase*>();
}

Scene::~Scene()
{
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

void Scene::Update()
{

}

void Scene::AddGameObject(GameObjectBase* _GameObject)
{
	m_GameObjectList.push_back(_GameObject);
}

void Scene::AddGameObject(Model* _Model)
{
	GameObjectBase* gameObject = new GameObject();

	gameObject->AddComponent(new ModelComponent(_Model));
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

