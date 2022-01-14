#pragma once
#include "Utils/State/StateSystem.h"
#include "Utils/Managers/UniverseManager.h"

class BulletState :
	public State
{
protected:
	UniverseManager* m_pUni_man;

public:
	BulletState(void (*func), std::string name) : State(name) { parent = func; m_pUni_man = (UniverseManager*)parent; }

	virtual void OnEnter();
	virtual void OnUpdate(const float dt);
	//virtual void OnExit();
};

