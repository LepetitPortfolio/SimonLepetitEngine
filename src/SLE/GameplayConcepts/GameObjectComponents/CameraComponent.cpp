#include "CameraComponent.h"
#include "../../Engine.h"
#include "../../Core/GlobalFunctionLibrary.h"

CameraComponent::CameraComponent(CameraBase* _CameraBase, Transform _LocalTransform) : LocationComponent(_LocalTransform)
{
	if (m_Camera)
	{
		m_Camera = _CameraBase;
	}
}

CameraComponent::~CameraComponent()
{
	if (m_Camera)
	{
		Desinit();
		delete m_Camera;
	}
}

void CameraComponent::Init(GameObjectBase* _GameObjectParent)
{
	LocationComponent::Init(_GameObjectParent);	
}

void CameraComponent::Desinit()
{
	LocationComponent::Desinit();
	DisableAction();
}

void CameraComponent::EnabledAction()
{
	LocationComponent::EnabledAction();

	if ((m_Camera) && (!m_Camera->IsUsed()))
	{
		Engine* engine = GlobalFunctionLibrary::GetEngine();

		if (engine)
		{
			engine->ChangeMainCamera(m_Camera);
		}
	}
}

void CameraComponent::DisableAction()
{
	LocationComponent::DisableAction();

	if ((m_Camera) && (m_Camera->IsUsed()))
	{
		Engine* engine = GlobalFunctionLibrary::GetEngine();

		if (engine)
		{
			engine->RemoveMainCamera();
		}
	}
}

