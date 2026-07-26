#include "GameObjectComponentBase.h"

uint64_t GameObjectComponentBase::GetUID()
{
	if (m_UID == 0)
	{
		m_UID = reinterpret_cast<uint64_t>(this);
	}

	return m_UID;
}


void GameObjectComponentBase::Enabled()
{
	Enabled(!m_Enabled);
}


void GameObjectComponentBase::Enabled(bool _Enabled)
{
	if(m_Enabled != _Enabled)
	{
		m_Enabled = _Enabled;
		if(m_Enabled)
		{
			EnabledAction();
		}
		else
		{
			DisableAction();
		}
	}
}