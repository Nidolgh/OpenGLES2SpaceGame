#pragma once

#include <string>
#include <map>

class State
{
public:
	void* parent; // empty type

	std::string m_state_name;

	State();
	State(std::string);

	virtual void OnEnter();
	virtual void OnUpdate(const float dt);
	virtual void OnExit();
};

class StateSystem
{
protected:
	std::map<std::string, State*> m_state_list;

public:
	State* m_pCurrent_state = nullptr;

	StateSystem();
	~StateSystem();

	void RegisterState(State*);

	void SwitchState(const std::string name);
	void OnUpdate(const float dt);
};