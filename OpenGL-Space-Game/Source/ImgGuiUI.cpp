#include "Headers/ImgGuiUI.h"

ImgGuiUI::ImgGuiUI()
{
}

ImgGuiUI::~ImgGuiUI()
{
}

bool ImgGuiUI::Init(const BaseWindow* win, const BaseInput* inp, const UniverseManager* u_man)
{
	m_pWindow = win;
	m_pInput = inp;
	m_pUniverse_man = u_man;

	// imgui setup
	const char* glsl_version = "#version 100";
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init(glsl_version);

	// it once	
	ImGui::SetNextWindowPos(ImVec2(200, 200));
	ImGui::SetNextWindowSize(ImVec2(400, 24 * 4));

	return true;
}

bool ImgGuiUI::Update(bool DoINeedTo, float DT)
{
	if (!DoINeedTo) return true; // if not updating there is no error so just return true
	
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(m_pWindow->GetWidth(), m_pWindow->GetHeight()); // window size, NOT resolution, this is important
	io.DeltaTime = DT;

	// we should also pass on mouse and key info into the IO here
	io.MousePos = ImVec2(m_pInput->m_mouse_data.PositionX, m_pInput->m_mouse_data.PositionY);
	io.MouseDown[0] = m_pInput->m_mouse_data.LeftButton; // left
	io.MouseDown[1] = m_pInput->m_mouse_data.RightButton; // right
	io.MouseDrawCursor = true;


	// get on with the text/windows	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame(); // do this at the start of any frame
	io.WantCaptureMouse = true;

	ImGui::SetNextWindowBgAlpha(0);

	ImGui::Begin("Game debug info");

	glm::vec3 p_force = m_pUniverse_man->GetActivePlayer()->GetEulerForce();
	glm::vec3 p_pos = m_pUniverse_man->GetActivePlayer()->GetTransform().GetPosition();
	glm::vec3 cam_pos = m_pUniverse_man->GetCamera()->get_world_pos_v3();
	glm::vec3 sky_pos = m_pUniverse_man->GetSkybox()->GetTransform().GetPosition();

	// https://stackoverflow.com/questions/41742142/limiting-fps-in-c, laptop seems to hit the 400 fps mark, or use deltatime to scale time
	frames++;
	totaltime += DT;
	if (totaltime > 1.0f)
	{
		//float fps = (1.0f / DT) * 1000.0f;
		cur_fps		= (frames / totaltime);
		cur_time	= totaltime;
		cur_frames	= frames;
		cur_dt		= DT;

		totaltime -= 1.0f;
		frames = 0;
	}
	ImGui::Text("FPS=%3.4f, deltatime:%f", cur_fps, cur_dt);

	if (ImGui::Button("Grant all artifacts"))
	{
		m_pUniverse_man->GetSpacePlayer()->m_the_one_total = 4; // 4 is the magic number
	}

	ImGui::Text("Player euler force: x:%f, y:%f, z%f", p_force.x, p_force.y, p_force.z);
	ImGui::Text("Player forwa force: %f", m_pUniverse_man->GetActivePlayer()->GetForwardForce());
	ImGui::Text("Player fov: %f", m_pUniverse_man->GetCamera()->get_fov());
	ImGui::Text("Active Player pos: x:%f, y:%f, z%f", p_pos.x, p_pos.y, p_pos.z);
	ImGui::Text("Cam. pos: x:%f, y:%f, z%f", cam_pos.x, cam_pos.y, cam_pos.z);
	ImGui::Text("Skybox pos: x:%f, y:%f, z%f", sky_pos.x, sky_pos.y, sky_pos.z);
	//ImGui::Text("pos_from_station: x:%f, y:%f, z%f", pos_from_station.x, pos_from_station.y, pos_from_station.z);
	//ImGui::Text("pos_stat_station: x:%f, y:%f, z%f", stat.x, stat.y, stat.z);

	if (ImGui::TreeNode("TOAO"))
	{
		ImGui::Text("%s", "Dion"); // the "%s", formats it to st
		ImGui::TreePop();
	}

	ImGui::End();
	return true; // we're not likely to do much that needs a false condition but pass a variable if you prefer
}

bool ImgGuiUI::Render(bool DoINeedTo)
{
	if (!DoINeedTo) return true; // if not updating there is no error so just return true

	ImGui::Render();
	// and finally render to the GPU	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return true; // remember GL returns int values, we ned to return bool
}
