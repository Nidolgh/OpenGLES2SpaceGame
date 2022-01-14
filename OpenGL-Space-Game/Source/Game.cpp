#include "../Headers/Game.h"

Game::Game()
{

}

Game::~Game()
{
	delete m_window;
	delete m_input;
}

void Game::Init()
{
#ifdef RASPBERRY
	m_window = new ARMWindow();
#else
	m_window = new XWindow();
#endif // RASPBERRY todo get system screen size for fullscreen window, perhaps with state to determin glfw from linux
	
	m_window->Init(1920, 1080, false);

	unsigned int width, height;
	m_window->GetRes(width, height);

	m_input = m_window->PrepareInput();
	m_input->Init(width, height);

	// register gamestates
	m_universe_manager.m_state_system.RegisterState(m_pBullet_state		= new BulletState		(&m_universe_manager, "BulletState"));
	m_universe_manager.m_state_system.RegisterState(m_pDock_state		= new DockState			(&m_universe_manager, "DockState"));
	m_universe_manager.m_state_system.RegisterState(m_pSpace_state		= new SpaceState		(&m_universe_manager, "SpaceState"));
	m_universe_manager.m_state_system.RegisterState(m_pTransition_state = new TransitionState	(&m_universe_manager, "TransitionState"));

	m_gui.Init(m_window, m_input, &m_universe_manager);

	m_resource_manager.Init(m_window);
	m_universe_manager.Init(&m_resource_manager);
}

void Game::Load()
{
	m_universe_manager.Load();
}

void Game::Update(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_universe_manager.Update(dt);
}

void Game::MainLoop()
{
	std::chrono::high_resolution_clock timer;
	std::chrono::high_resolution_clock::time_point start = timer.now(), stop;
	float deltatime = 0;
	while (!m_window->ShouldWindowClose())
	{
		// changed the way deltatime gets calculated using chrono, old way I could not quite get working on windows
		stop = timer.now();
		std::chrono::duration<double> elapsed = stop - start;
		deltatime = elapsed.count();
		start = stop;
		
		Update(deltatime);

		m_gui.Update(true, deltatime);
		m_gui.Render(true);

		// after our draw we need to swa p buffers to display
		m_window->SwapBuffer();
		m_input->PollEvents();

		HandleInput();
	}
}

void Game::HandleInput()
{
	// todo make input handling accross platform more friendlier, input classes werent made for nothing
	Player* the_player = m_universe_manager.GetActivePlayer();

	// update player variables, should make own enum collection?
#ifdef RASPBERRY
	the_player->m_up_key				= m_input->TestKey(KEY_W) || m_input->TestKey(KEY_UP);
	the_player->m_down_key				= m_input->TestKey(KEY_S) || m_input->TestKey(KEY_DOWN);
	the_player->m_left_key				= m_input->TestKey(KEY_A) || m_input->TestKey(KEY_LEFT);
	the_player->m_right_key				= m_input->TestKey(KEY_D) || m_input->TestKey(KEY_RIGHT);
	the_player->m_space_pressed			= m_input->TestKey(KEY_SPACE);
	the_player->m_boost_key				= m_input->TestKey(KEY_LEFTSHIFT);
	the_player->m_stabilize_ship_key	= m_input->TestKey(KEY_Z);
	the_player->m_hor_mouse_vel			= m_input->m_mouse_data.RelX * 0.04f;
	the_player->m_ver_mouse_vel			= m_input->m_mouse_data.RelY * 0.04f;

	the_player->m_set_forward_key = m_input->TestKey(KEY_P);

	if (m_input->TestKey(KEY_ESC))
		m_window->SetWindowShouldClose(true);

	if (m_input->TestKey(KEY_C))
	{
		// only can dock if we are allowed to, should move to universemanager?
		if (m_universe_manager.GetSStation()->m_is_dockable &&
			m_universe_manager.m_state_system.m_pCurrent_state->m_state_name == "SpaceState")
			m_universe_manager.m_state_system.SwitchState("DockState");
	}

	// mouse velocity resetting 
	if (m_input->m_mouse_data.RelX > FLT_EPSILON || m_input->m_mouse_data.RelX < -FLT_EPSILON)
		m_input->m_mouse_data.RelX = 0.0f;
	if (m_input->m_mouse_data.RelY > FLT_EPSILON || m_input->m_mouse_data.RelY < -FLT_EPSILON)
		m_input->m_mouse_data.RelY = 0.0f;

#else
	the_player->m_up_key				= m_input->TestKey(GLFW_KEY_W) || m_input->TestKey(GLFW_KEY_UP);
	the_player->m_down_key				= m_input->TestKey(GLFW_KEY_S) || m_input->TestKey(GLFW_KEY_DOWN);
	the_player->m_left_key				= m_input->TestKey(GLFW_KEY_A) || m_input->TestKey(GLFW_KEY_LEFT);
	the_player->m_right_key				= m_input->TestKey(GLFW_KEY_D) || m_input->TestKey(GLFW_KEY_RIGHT);
	the_player->m_space_pressed			= m_input->TestKey(GLFW_KEY_SPACE);
	the_player->m_boost_key				= m_input->TestKey(GLFW_KEY_LEFT_SHIFT);
	the_player->m_stabilize_ship_key	= m_input->TestKey(GLFW_KEY_Z);
	the_player->m_hor_mouse_vel			= m_input->m_mouse_data.RelX * 0.05f;
	the_player->m_ver_mouse_vel			= m_input->m_mouse_data.RelY * 0.05f;

	the_player->m_set_forward_key = m_input->TestKey(GLFW_KEY_P);

	if (m_input->TestKey(GLFW_KEY_ESCAPE))
		m_window->SetWindowShouldClose(true);

	if (m_input->TestKey(GLFW_KEY_TAB))
		m_input->SwitchMouseMode();

	if (m_input->TestKey(GLFW_KEY_C))
	{
		// only can dock if we are allowed to, should move to universemanager?
		if (m_universe_manager.GetSStation()->m_is_dockable && 
				m_universe_manager.m_state_system.m_pCurrent_state->m_state_name == "SpaceState")
			m_universe_manager.m_state_system.SwitchState("DockState");
	}
#endif // RASPBERRY
}