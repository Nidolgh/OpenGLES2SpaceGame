#pragma once

// https://sourceforge.net/p/predef/wiki/OperatingSystems/
#ifdef RASPBERRY
#include "Utils/Window/ARMWindow.h"
#include "Utils/Input/ARMInput.h"
#else
#include "Utils/Window/XWindow.h"
#include "Utils/Input/XInput.h"
#endif // RASPBERRY

#include <chrono>
#include "glm/glm.hpp"

#include "ImgGuiUI.h"

#include "Utils/Managers/ResourceManager.h"
#include "Utils/Managers/UniverseManager.h"

#include <Utils/State/GameStates/BulletState.h>
#include <Utils/State/GameStates/DockState.h>
#include <Utils/State/GameStates/SpaceState.h>
#include <Utils/State/GameStates/TransitionState.h>

class Game
{
protected:
	ResourceManager m_resource_manager;
	UniverseManager m_universe_manager;

	BaseWindow* m_window; 
	BaseInput* m_input; // keyboard and mouse input

	// gui
	ImgGuiUI m_gui;

	State* m_pBullet_state;
	State* m_pDock_state;
	State* m_pSpace_state;
	State* m_pTransition_state;

public:
	Game();
	virtual ~Game();

	void Init();
	void Load();

	void Update(float dt);
	void MainLoop();
	void HandleInput();
};