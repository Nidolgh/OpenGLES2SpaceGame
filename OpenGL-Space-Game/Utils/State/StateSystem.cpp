#include "StateSystem.h"

State::State()
{
}

State::State(std::string name)
{
	m_state_name = name;
}

void State::OnEnter()
{
	printf("Entered '%s'\n", m_state_name.c_str());
}

void State::OnUpdate(const float dt)
{
	//parent(dt); // exectu passed function pointer
}

void State::OnExit()
{
	printf("Exited '%s'\n", m_state_name.c_str());
}

StateSystem::StateSystem()
{
}

StateSystem::~StateSystem()
{
}

void StateSystem::RegisterState(State* the_state)
{
	m_state_list.insert({ the_state->m_state_name, the_state });
}

void StateSystem::SwitchState(const std::string name)
{
	if (m_pCurrent_state != nullptr)
		m_pCurrent_state->OnExit();
	std::map< std::string, State* >::iterator it;
	it = m_state_list.find(name);
	if (it != m_state_list.end())
	{
		m_pCurrent_state = it->second;
	}
	if (m_pCurrent_state != nullptr)
		m_pCurrent_state->OnEnter();
	else
		printf("STATESYSTEM::SWITCHSTATE something has gone terribly wrong\n");
}

void StateSystem::OnUpdate(const float dt)
{
	m_pCurrent_state->OnUpdate(dt);
}
