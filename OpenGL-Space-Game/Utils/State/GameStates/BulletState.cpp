#include "BulletState.h"

void BulletState::OnEnter()
{
	// first deallocate space objects memory
	m_pUni_man->ClearSpaceObjects();

	m_pUni_man->SwapPlayerToBullet();

	// first set bullet player pos to stations so we can do stuff, should
	m_pUni_man->GetBulletPlayer()->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, -200.0f));

	m_pUni_man->GetBulletScene()->Init(m_pUni_man->GetBulletPlayer(), 
		m_pUni_man->GetResManager());
}

void BulletState::OnUpdate(const float dt)
{
	m_pUni_man->UpdateBulletObjects(dt);
}
