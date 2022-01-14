#include "SpaceState.h"

void SpaceState::OnEnter()
{
	// m_pUni_man->SwapPlayerToBullet(); // set active player to space
}

void SpaceState::OnUpdate(const float dt)
{
	m_pUni_man->UpdateSpaceObjects(dt);
}