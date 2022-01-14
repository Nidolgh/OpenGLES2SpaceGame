#pragma once
#include "Utils/State/StateSystem.h"
#include "Utils/Managers/UniverseManager.h"

class DockState :
	public State
{
protected:
	UniverseManager* m_pUni_man;

	float m_tt = 0.0f;
	float m_check_dist_interval = 0.5f;

public:
	DockState(void (*func), std::string name) : State(name) { parent = func; m_pUni_man = (UniverseManager*)parent; }

	//virtual void OnEnter();
	virtual void OnUpdate(const float dt);
	//virtual void OnExit();
};

