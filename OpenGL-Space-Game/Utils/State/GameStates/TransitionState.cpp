#include "TransitionState.h"

void TransitionState::OnUpdate(const float dt)
{
	m_pUni_man->m_state_system.SwitchState("BulletState");
}