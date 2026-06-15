#include "GameObjectComponentBase.h"

void GameObjectComponentBase::Enabled(bool _Enabled)
{
	if(m_Enabled != _Enabled)
	{
		m_Enabled = _Enabled;
		if(m_Enabled)
		{
			Init();
		}
		else
		{
			Desinit();
		}
	}
}