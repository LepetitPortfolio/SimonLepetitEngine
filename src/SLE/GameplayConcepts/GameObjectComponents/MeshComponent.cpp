#include "MeshComponent.h"
#include "../GameObjectBase.h"


MeshComponent::MeshComponent(Mesh* _Mesh, Transform _LocalTransform) : LocationComponent(_LocalTransform)
{
	if(_Mesh != nullptr)
	{
		m_Mesh = _Mesh;
	}
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::SetMesh(Mesh* _Mesh)
{
	if(_Mesh != nullptr)
	{
		m_Mesh = _Mesh;		
	}
}

void MeshComponent::Init(GameObjectBase* _GameObjectParent)
{
	LocationComponent::Init(_GameObjectParent);

}

void MeshComponent::Desinit()
{
	LocationComponent::Desinit();

	if (!m_Mesh)
	{
		return;
	}
}


void MeshComponent::Draw(VulkanFrameInfo& _FrameInfo)
{
	if (m_Mesh)
	{
		m_Mesh->Draw( GetWorldTransform(), _FrameInfo);
	}
}

void MeshComponent::EnabledAction()
{
	LocationComponent::EnabledAction();

	if (m_GameObjectParent)
	{
		m_GameObjectParent->AddDrawCallback([this](VulkanFrameInfo _FrameInfo) { this->Draw(_FrameInfo); });
	}
}

void MeshComponent::DisableAction()
{
	LocationComponent::DisableAction();

	if (m_GameObjectParent)
	{
		m_GameObjectParent->RemoveDrawCallback([this](VulkanFrameInfo _FrameInfo) { this->Draw(_FrameInfo); });
	}
}

