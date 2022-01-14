#include "DockState.h"

//virtual void OnEnter();

void DockState::OnUpdate(const float dt)
{
	m_pUni_man->UpdateSpaceObjects(dt);
	m_pUni_man->GetSpacePlayer()->UpdateDock(dt);

	m_tt += dt;
	if (m_tt > m_check_dist_interval)
	{
		m_tt = 0.0f;
		if (m_pUni_man->GetSpacePlayer()->DistFromDock() < 0.8f)
		{
			m_pUni_man->m_state_system.SwitchState("TransitionState");
		}
	}
}
