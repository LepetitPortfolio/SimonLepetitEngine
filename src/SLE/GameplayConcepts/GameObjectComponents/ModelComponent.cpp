#include "ModelComponent.h"


ModelComponent::ModelComponent(Model* _Model)
{
	if(_Model != nullptr)
	{
		m_Model = _Model;
	}
}

ModelComponent::~ModelComponent()
{
}

void ModelComponent::SetModel(Model* _Model)
{
	if(m_Model != nullptr)
	{
		m_Model->ShowInGame(false);
	}

	if(_Model != nullptr)
	{
		m_Model = _Model;
		if (m_Enabled)
		{
			Init();
		}
	}
}

void ModelComponent::Init()
{
	if (!m_Model)
	{
		return;
	}
	m_Model->ShowInGame(true);
}

void ModelComponent::Desinit()
{
	if (!m_Model)
	{
		return;
	}
	m_Model->ShowInGame(false);
}

void ModelComponent::Update()
{
}

